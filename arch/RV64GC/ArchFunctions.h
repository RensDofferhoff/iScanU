#pragma once


#include "ArchProperties.h"
#include "Injector.h"

void setState(mcontext_t* context, reg_t pc, reg_t regs[]);
void setStatePtrace(user_regs_struct* context, reg_t pc, reg_t regs[]);


void initDisassembler(Injector* scanner);
bool disassemble(Injector* scanner, instr_t opcode, void* info);


void setRecoveryData(Injector* scanner);
void recoverState();
void clearCache(void* begin, void* end);

bool vleExhaustive(Injector* data, instr_t* nextInstruction);
bool riscvcOnlyFetch(Injector* data, instr_t* nextInstruction);
void riscvLengthLogAnalysis(Injector* data);
void basicAnalysisAndSizeCheckRiscv(Injector* data);
