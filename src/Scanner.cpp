#include "Scanner.h"
#include "Blacklist.h"
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include "ArchProperties.h"
#include "ArchFunctions.h"
#include "Handlers.h"
#include "ScannerManager.h"
#include "Utility.h"
#include "Analyser.h"
#include "Feeder.h"
#include <sched.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <elf.h>
#include <sys/mman.h>

Scanner* data = NULL;

void writeInstruction(uint8_t* addr, instr_t instr) {
    memcpy(addr, &instr, data->currentInstructionSize);
}


void writeStdPage(uint8_t* addr) {
    memcpy(addr, stdPage, pageSize);
}


void* initScanner(void* ptr) {
	Scanner* scannerData = (Scanner*)ptr;
    data = scannerData;

    //create instructionpage and surrounding guards
    uint8_t* instructionPage = (uint8_t*)mmap(NULL, 2 * maxArchOffset + pageSize, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS | MAP_NORESERVE, 0, 0) + maxArchOffset;
    mprotect(instructionPage, pageSize, PROT_EXEC | PROT_WRITE | PROT_READ);
    memcpy(instructionPage, stdPage, pageSize);
    data->instructionPage = instructionPage;
    data->instructionPointer = instructionPage + pageSize - data->currentInstructionSize;

    setAltStack(data->altStack);
    initDisassembler(data);
    data->isReady = true;
	pause(); //Wait for start signal
	return 0;
}

void setAltStack(stack_t& altStack) {
    altStack.ss_sp = (uint8_t*)mmap(NULL, 16 * pageSize, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS | MAP_NORESERVE, 0, 0);
	altStack.ss_flags = 0;
	altStack.ss_size = 8 * pageSize;
	int error = sigaltstack(&altStack, NULL);
	if (error) {
		printf("sig stack error\n");
	}
}


//==============================================================================
//ptrace methods:

int initScannerPtrace(void* ptr) {
    Scanner* scannerData = (Scanner*)ptr;
    scannerData->instructionPointer = (uint8_t*)mmap(NULL, pageSize, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, 0, 0);
    initDisassembler(scannerData);

    scannerData->isReady = true;
    pause(); //Wait for start signal
    void* traceeStack = (uint8_t*)mmap(NULL, pageSize, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS | MAP_NORESERVE, 0, 0) + pageSize;
    int wstatus;
    int pid = clone(&traceeSetup, traceeStack, CLONE_VM, NULL);
    waitpid(-1, &wstatus, 0); //Wait for the tracee init signal

    writeTimestamp(scannerData->outputFD);
    std::string start = "======= start of run =======\n";
    write(scannerData->outputFD, start.c_str(), start.size());
    sleep(1); //affinity settle
    ptraceLoop(pid, scannerData);
    return 0;
}

int traceeSetup(void* ptr) {
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    raise(SIGSTOP); //allow parent to observe sigstop
    return -1; //should never reach
}

void ptraceLoop(pid_t pid, Scanner* scannerData) {
    siginfo_t sig;
    bool finished = false;
    int wstatus;
    user_regs_struct resultState;
    user_regs_struct startState;
    setStatePtrace(&startState, (reg_t)scannerData->instructionPointer, NULL);

    iovec startIovec;
    startIovec.iov_base = (void*) &startState;
    startIovec.iov_len = sizeof(user_regs_struct);
    iovec resultIovec;
    resultIovec.iov_base = (void*) &resultState;
    resultIovec.iov_len = sizeof(user_regs_struct);

    data = scannerData;
    data->lastInfo = &sig;
    data->lastContext = &resultState;

    if (blacklist.search(data->currentInstruction)) {
         bool finished = (*fetchInstruction)(data, &data->currentInstruction);
         if (finished) {
             stopWorker(data);
         }
     }

    while(!finished) {
        //set all GPRs to 0 and PC to instruction address
        ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &startIovec);
        //write instruction
        memcpy(data->instructionPointer, &data->currentInstruction, instructionSize);
        clearCache(data->instructionPointer, data->instructionPointer + instructionSize);
        //Force single step and catch exception on entry
        ptrace(PTRACE_SINGLESTEP, pid, 0, 0);
        waitpid(-1, &wstatus, 0);
        //Force single step and catch resulting exception of execution on exit
        ptrace(PTRACE_SINGLESTEP, pid, 0, 0);
        waitpid(-1, &wstatus, 0);
        ptrace(PTRACE_GETSIGINFO, pid, 0, &sig);
        ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &resultIovec); //gets resulting state
        data->lastSigno = sig.si_signo;
        (*analyse)(data);
        data->numInstrExec++;
        finished = (*fetchInstruction)(data, &data->currentInstruction);
    }
    kill(pid, SIGKILL);
    data->isStopped = true;
    writeTimestamp(data->outputFD);
    std::string start = "======= end of run =======\n";
    write(data->outputFD, start.c_str(), start.size());
    exit(0);
}
