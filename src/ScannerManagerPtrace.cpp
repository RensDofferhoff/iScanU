#include "ScannerManagerPtrace.h"
#include "ArchProperties.h"
#include "Handlers.h"
#include "Utility.h"
#include <signal.h>
 #include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <sched.h>
#include <fcntl.h>

ScannerManagerPtrace::ScannerManagerPtrace(int _numThreads, uint64_t first, uint64_t last) : ScannerManager(_numThreads) {
    registerHandlers();
    createCriticalOutputDir();
    checkForHang = ssHang;
    managerFD = openCriticalOutputFile("results/manager");
    performanceLogFD = openCriticalOutputFile("results/performance");
    performanceLogSlowFactor = 10; //once every 10 alarms
    performanceLogCount = 0;

    if (last - first < (uint64_t)numThreads && last - first > 0) {
        numThreads = last - first;
    }
    uint64_t instrPerThread = (last - first) / numThreads;
    reg_t startInstruction = first;
    reg_t finalInstruction = first + instrPerThread;
    //setup scanner units
    for (int i = 0; i < numThreads; ++i) {
        Scanner* scannerData = (Scanner*)mmap(NULL, sizeof(Scanner), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS | MAP_NORESERVE, 0, 0);
        scannerData->currentInstruction = startInstruction;
        scannerData->finalInstruction = finalInstruction;
        scannerData->outputFD = openCriticalOutputFile("results/thread" + std::to_string(i));
        scannerData->hangOutputFD = openCriticalOutputFile("results/hangs" + std::to_string(i));
        scannerData->debugFD = openCriticalOutputFile("results/debug" + std::to_string(i));
        localScannerInit(scannerData);

        pid_t pid = fork();
        if(pid == -1) {
            printf("Failed to create ptrace scanner\n");
            printf("%s\n", strerror(errno));
            exit(-1);
        }
        if(pid == 0) {
            initScannerPtrace(scannerData);
            exit(-1); //should never reach this point
        }
        scannerData->workerID = i;
        threadDataMap.insert({pid, scannerData});

        startInstruction = finalInstruction + 1;
        finalInstruction += instrPerThread;
        if (i == numThreads - 1) {
            scannerData->finalInstruction = last;
        }
    }

}

ScannerManagerPtrace::~ScannerManagerPtrace() {

}

void ScannerManagerPtrace::localScannerInit(Scanner* data) {
    data->oldNumInstrExec = 0;
    data->numInstrExec = 0;
    data->lastPerformanceExec = 0;
    data->isReady = false;
    data->isStopped = false;
    data->managerID = getpid();
}

void ScannerManagerPtrace::runScanners() {
    bool allReady = false;
    while(!allReady) {
        allReady = true;
        for(const auto& thread : threadDataMap) {
            if(!thread.second->isReady) {
                allReady = false;
                break;
            }
        }
    }
    for(const auto& thread : threadDataMap) {
        kill(thread.first, SIGUSR1);
    }
    alarm(1);
}

void ScannerManagerPtrace::registerHandlers() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

	sa.sa_sigaction = alarmHandler;
	sigaction(SIGALRM, &sa, NULL);

    sa.sa_sigaction = startHandlerPtrace;
	sigaction(SIGUSR1, &sa, NULL);

    sa.sa_sigaction = hangHandlerPtrace;
	sigaction(SIGUSR2, &sa, NULL);
}
