#include "ScannerManager.h"
#include "ScannerManagerMC.h"
#include "ScannerManagerPtrace.h"
#include "Scanner.h"
#include "Handlers.h"
#include "Feeder.h"
#include "Analyser.h"
#include "Utility.h"
#include "ArchProperties.h"
#include "ArchFunctions.h"
#include "Utility.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    const int MAX_THREADS_MEMCAGE = 96;
    int numThreads = 1;
    int numCores = 1;
    bool setAffinity = false;
    uint64_t firstInstruction = 0;
    uint64_t lastInstruction = MaxNumInstructions - 1;
    method = Method::MEMCAGE;
    //set fetch and analysis function pointers
    analyse = &basicAnalysis;
    fetchInstruction = &exhaustive;
    if(variableLengthEncoding) {
        lastInstruction = ((reg_t)1 << (instructionSize * 8)) - 1;
    }

    uint64_t arg;
    int option;
    while ((option = getopt(argc, argv, "j:b:e:a:mpkid")) != -1) {
        switch (option) {
        case 'j':
            arg = strtoull(optarg, NULL, 0);
            if (arg < 1) {
                numThreads = 1;
                printf("Invalid amount of threads specified, using 1 threads instead\n");
            }
            else {
                numThreads = arg;
            }
            break;
        case 'b':
            arg = strtoull(optarg, NULL, 0);
            if (arg <= lastInstruction) {
                firstInstruction = arg;
            }
            break;
        case 'c':
            break;
        case 'e':
            arg = strtoull(optarg, NULL, 0);
            if (arg <= lastInstruction && arg >= firstInstruction) {
                lastInstruction = arg;
            }
            break;
        case 'm':
            method = Method::MEMCAGE;
            break;
        case 'p':
            if(!archSingleStepPtrace) {
                printf("Ptrace method not available for this architecture\n");
                exit(-1);
            }
            method = Method::PTRACE;
            break;
        case 'k':
            method = Method::KERNELMOD;
            break;
        case 'i':
            analyse = &insnAnalysis;
            break;
        case 'd':
            debugMode = true;
            break;
        case 'a':
            numCores = strtoull(optarg, NULL, 0);
            setAffinity = true;
            break;
        }

    }

    ScannerManager* scannerManager;
    switch (method) {
        case Method::MEMCAGE:
            if(numThreads > MAX_THREADS_MEMCAGE) {
                numThreads = MAX_THREADS_MEMCAGE;
                printf("Invalid amount of threads specified, using %u threads instead\n", MAX_THREADS_MEMCAGE);
            }
            scannerManager = new ScannerManagerMC(numThreads, firstInstruction, lastInstruction);
            break;
        case Method::PTRACE: scannerManager = new ScannerManagerPtrace(numThreads, firstInstruction, lastInstruction); break;
        case Method::KERNELMOD: scannerManager = new ScannerManagerMC(numThreads, firstInstruction, lastInstruction); break; //TODO:
    }

    if(setAffinity) {
        scannerManager->setAffinity(numCores);
    }
    scannerManager->runScanners();
    while(true) {
        pause();
    }
}
