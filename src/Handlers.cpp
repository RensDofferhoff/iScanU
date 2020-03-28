#include "Handlers.h"
#include "Blacklist.h"
#include "ArchProperties.h"
#include "Scanner.h"
#include "ScannerManager.h"
#include "Utility.h"
#include "Analyser.h"
#include "Feeder.h"
#include "string.h"
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "ArchFunctions.h"
#include <string>

//Memory cage handlers
//==============================================================================

void faultHandler(int signo, siginfo_t* info, void* context) {
    recoverState();
    data->lastSigno = signo;
    data->lastInfo = info;
    data->lastContext = context;
    (*analyse)(data);
    instr_t nextInstruction;
    bool finished = (*fetchInstruction)(data, &nextInstruction);
    data->numInstrExec++;
    if (finished) {
        stopWorker(data);
    }
    if(maxPcRelativeWriteOffset) {
        writeStdPage(data->instructionPage);
    }
    writeInstruction(data->instructionPointer, nextInstruction);
    data->currentInstruction = nextInstruction;
    ucontext_t* ucontext = (ucontext_t*)context;
    setState(&ucontext->uc_mcontext, (reg_t)data->instructionPointer, NULL);
    clearCache(data->instructionPage, data->instructionPage + pageSize);
}


void alarmHandler(int signo, siginfo_t* info, void* context) {
	alarm(0);
    //log some performance metrics
    if(performanceLogCount == performanceLogSlowFactor - 1) {
        int insn = 0;
        int temp;
        std::string output = "===== start of performance log =====\n";
        for(auto& thread : threadDataMap) {
            if(thread.second->isStopped) continue;
            temp = thread.second->numInstrExec - thread.second->lastPerformanceExec;
            insn += temp;
            output += "thread " + std::to_string((uint64_t)thread.second->workerID) + ": " + std::to_string(temp) + "\n";
            thread.second->lastPerformanceExec = thread.second->numInstrExec;
        }
        output += "Total: " + std::to_string(insn) + "\n";
        write(performanceLogFD, output.c_str(), output.size());
        writeTimestamp(performanceLogFD);
    }
    performanceLogCount++;
    performanceLogCount %= performanceLogSlowFactor;


    //check for hang condition and fix if occured & check for completion
    bool finished = true;
    for(auto& thread : threadDataMap) {
        if(!thread.second->isStopped) {
            finished = false;
        }

        uint64_t currentExecuted = thread.second->numInstrExec;
        uint64_t oldExecuted = thread.second->oldNumInstrExec;
        if(checkForHang && currentExecuted == oldExecuted && !thread.second->isStopped) {
            kill(thread.first, SIGUSR2);
        }
        thread.second->oldNumInstrExec = currentExecuted;
    }
    if(finished) {
        std::string output = "finished run\n";
        write(managerFD, output.c_str(), output.size());
        exit(0);
    }
	alarm(1);
}


void entryHandler(int signo, siginfo_t* info, void* context) {
    setRecoveryData(data);
    writeTimestamp(data->outputFD);
    std::string start = "======= start of run =======\n";
    write(data->outputFD, start.c_str(), start.size());

    if (blacklist.search(data->currentInstruction)) {
    	bool finished = (*fetchInstruction)(data, &data->currentInstruction);
    	if (finished) {
            stopWorker(data);
    	}
    }

    writeInstruction(data->instructionPointer, data->currentInstruction);
    ucontext_t* ucontext = (ucontext_t*)context;
    //Store known good state for use in scan loop
    setState(&ucontext->uc_mcontext, (reg_t)data->instructionPointer, NULL);
    clearCache(data->instructionPage, data->instructionPage + pageSize);
}

void hangHandler(int signo, siginfo_t* info, void* context) {
    recoverState();
    std::string output = "HANG " + std::to_string(data->currentInstruction) + "\n";
    write(data->hangOutputFD, output.c_str(), output.size());
    data->lastSigno = SIGSEGV;
    data->lastInfo = info;
    data->lastContext = context;
    (*analyse)(data);

    memcpy(data->instructionPage, stdPage, pageSize);
	bool finished = (*fetchInstruction)(data, &data->currentInstruction);
    if(finished) {
        stopWorker(data);
    }
	writeInstruction(data->instructionPointer, data->currentInstruction);

	ucontext_t* ucontext = (ucontext_t*)context;
    setState(&ucontext->uc_mcontext, (reg_t)data->instructionPointer, NULL);
    clearCache(data->instructionPage, data->instructionPage + pageSize);
}

//==============================================================================
//Ptrace Handlers

void startHandlerPtrace(int signo, siginfo_t* info, void* context) {

}

void hangHandlerPtrace(int signo, siginfo_t* info, void* context) {
    std::string output = "HANG " + std::to_string(data->currentInstruction) + "\n";
    write(data->hangOutputFD, output.c_str(), output.size());
    data->lastSigno = SIGSEGV;
    data->lastInfo = info;
    data->lastContext = context;
    (*analyse)(data);

	bool finished = (*fetchInstruction)(data, &data->currentInstruction);
    if(finished) {
        printf("stopping from hang\n");
        stopWorker(data);
    }
	writeInstruction(data->instructionPointer, data->currentInstruction);

	ucontext_t* ucontext = (ucontext_t*)context;
    setState(&ucontext->uc_mcontext, (reg_t)data->instructionPointer, NULL);
    clearCache(data->instructionPage, data->instructionPage + pageSize);
}
