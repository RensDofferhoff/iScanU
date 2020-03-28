#pragma once

#include <unistd.h>
#include <cstdint>
#include <ucontext.h>

typedef uint32_t instr_t;
typedef uint64_t reg_t;
typedef int user_regs_struct;

class LowMemoryBlacklist; //forward declaration;

static const bool archSingleStep = false; //there is an riscv external debug extension that might be worth some researching
static const bool archSingleStepPtrace = false;
static const bool ssHang = false;

static const int maxArchOffset = 1024 * 1024;
static const int maxPcRelativeWriteOffset = 0;

static const int numGPRegs = 32;
static const int regBytes = 8;
static const int registerFileSize = numGPRegs * regBytes;

static const bool variableLengthEncoding = true;
static const size_t instructionSize = 2;
static const size_t instructionSizeUC = 4; //only used when vle == true
static const reg_t MaxNumInstructions = (reg_t)1 << (instructionSizeUC * 8);

static const size_t fillerInstructionSize = instructionSize;
static const instr_t fillerInstruction = 0b1001000000000010; //C.ebreak

static const int pageSize = 4096; //should move to system properties at some point
extern LowMemoryBlacklist blacklist;
