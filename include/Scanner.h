#pragma once

#include <capstone/capstone.h>
#include <signal.h>
#include <sys/types.h>
#include "ArchProperties.h"

//Struct contains all state related to a scanner unit.
//Stores progress for manager
struct Scanner {
    bool isReady;
    bool isStopped;
    pid_t workerID;
    uint64_t managerID;
    stack_t altStack;
    uint8_t* instructionPage;
    uint8_t* instructionPointer;

    instr_t currentInstruction;
    instr_t finalInstruction;
    size_t currentInstructionSize;
    bool sizeError;

    uint64_t oldNumInstrExec;
    uint64_t numInstrExec;
    uint64_t lastPerformanceExec;

    csh capstoneHandle;
    cs_insn* capstoneInstruction;

    int outputFD;
    int hangOutputFD;
    int debugFD;

    int lastSigno;
    siginfo_t* lastInfo;
    void* lastContext;

};

extern Scanner* data;

void writeInstruction(uint8_t* addr, instr_t instr);

void writeStdPage(uint8_t* addr);

void* initScanner(void* ptr);

//Sets alternative stack address for signal handling
void setAltStack(stack_t& altStack);

//==============================================================================
//ptrace specific methods:

int initScannerPtrace(void* ptr);

//Executes main scan loop of the ptrace method
void ptraceLoop(pid_t pid, Scanner* ScannerData);

int traceeSetup(void* ptr);

void writeInstructionPtrace(pid_t pid, uint8_t* addr, instr_t instr);
