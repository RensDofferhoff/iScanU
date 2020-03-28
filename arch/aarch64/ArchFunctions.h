#pragma once

#include "ArchProperties.h"
#include "Scanner.h"

void setState(mcontext_t* context, reg_t pc, reg_t regs[]);
void setStatePtrace(user_regs_struct* context, reg_t pc, reg_t regs[]);


void initDisassembler(Scanner* scanner);
bool disassemble(Scanner* scanner, instr_t opcode, void* info);

void setRecoveryData(Scanner* scanner);
void recoverState();
void clearCache(void* begin, void* end);
