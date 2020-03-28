#pragma once

#include <unistd.h>
#include <cstdint>
#include <ucontext.h>

//Integer type large enough to hold an instruction
typedef uint32_t instr_t;
//Integer type large enough to hold register values
typedef uint64_t reg_t;

class Blacklist; //forward declaration;

//Does ISA have single step capability?
static const bool archSingleStep = true;
//Does ISA have ptrace single step support?
static const bool archSingleStepPtrace = true;
//Can hangs occur even when using the ptrace method?
static const bool ssHang = false;

//Maximum offset a pc relative jump can have in bytes 
static const int maxArchOffset = 128 * 1024 * 1024;
//Maximum offset of a pc relative write. Set 0 if not possible.
static const int maxPcRelativeWriteOffset = 0;

static const int numGPRegs = 31;
static const int regBytes = 8;
static const int registerFileSize = numGPRegs * regBytes;
static const uint64_t pstate = 0x80000000;

//Is the ISA using a hybrid length encoding 
static const bool variableLengthEncoding = false;
//Smallest instruction size
static const size_t instructionSize = 4;
//Largest instruction size
static const size_t instructionSizeUC = 4; 
static const reg_t MaxNumInstructions = (reg_t)1 << (instructionSizeUC * 8);

static const size_t fillerInstructionSize = instructionSize;
static const instr_t fillerInstruction = 0x16000000; //C.ebreak

static const int pageSize = 4096; //should move to system properties at some point
extern Blacklist blacklist;
