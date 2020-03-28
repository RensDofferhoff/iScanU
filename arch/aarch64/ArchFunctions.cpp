#include "ArchFunctions.h"
#include <capstone/capstone.h>
#include <string.h>

void setState(mcontext_t* context, reg_t pc, reg_t regs[]) {
    memset((void*)context->regs, 0, registerFileSize);
    context->sp = 0;
	context->pc = pc;
    context->pstate = pstate;
}

void setStatePtrace(user_regs_struct* context, reg_t pc, reg_t regs[]) {
    memset((void*)context->regs, 0, registerFileSize);
    context->sp = 0;
	context->pc = pc;
    context->pstate = pstate;
}


void initDisassembler(Scanner* scanner) {
    if (cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &scanner->capstoneHandle) != CS_ERR_OK) {
        printf("failed to init capstone\n");
        exit(-1);
    }
    scanner->capstoneInstruction = cs_malloc(scanner->capstoneHandle);
}

bool disassemble(Scanner* scanner, instr_t opcode, void* info) {
    const uint8_t* firstInstructionByte = (uint8_t*)scanner->instructionPointer;
    uint64_t firstByteOffset = 0;
    uint64_t size = scanner->currentInstructionSize;
    bool isValid = cs_disasm_iter(scanner->capstoneHandle, &firstInstructionByte, &size, &firstByteOffset, scanner->capstoneInstruction);
    return isValid;
}


void recoverState() {

}

void setRecoveryData(Scanner* scanner) {

}

void clearCache(void* begin, void* end) {
    __builtin___clear_cache(begin, end);
}
