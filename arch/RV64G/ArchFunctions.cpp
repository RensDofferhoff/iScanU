#include "ArchFunctions.h"
#include "Blacklist.h"
#include <capstone/capstone.h>
#include <string.h>
#include <ucontext.h>

void setState(mcontext_t* context, reg_t pc, reg_t regs[]) {
    memset((void*)(context->__gregs) , 0, registerFileSize);
    context->__gregs[REG_PC] = pc;
}

void setStatePtrace(user_regs_struct* context, reg_t pc, reg_t regs[]) {
    exit(-1);
}


void initDisassembler(Scanner* scanner) {
    if (cs_open(CS_ARCH_RISCV, CS_MODE_RISCV64, &scanner->capstoneHandle) != CS_ERR_OK) {
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
    reg_t sp = 0;
    asm("mv %0, x2" : "=r" (sp) );
    reg_t i = sp + 4 * pageSize;
    reg_t* recoveryAddress = (reg_t*)(i - i % (4 * pageSize));

    reg_t gp = *(recoveryAddress);
    asm("mv x3, %0" : : "r" (gp));
    reg_t tp = *(recoveryAddress + 1);
    asm("mv x4, %0" : : "r" (tp));
}

void setRecoveryData(Scanner* scanner) {
    reg_t sp;
    asm("mv %0, x2" : "=r" (sp) );
    reg_t recoveryAddress = (reg_t)(sp) + 4 * pageSize;
    reg_t* alignedRecoveryAddress = (reg_t*)(recoveryAddress - recoveryAddress % (4 * pageSize));

    reg_t gp, tp;
    asm("mv %0, x3" : "=r" (gp));
    *(alignedRecoveryAddress) = gp;
    asm("mv %0, x4" : "=r" (tp));
    *(alignedRecoveryAddress + 1) = tp;
}


void clearCache(void* begin, void* end) {
    asm("fence.i" : : );
}


bool riscvgOnlyFetch(Scanner* data, instr_t* nextInstruction) {
    instr_t tempInstruction;
    if(data->currentInstruction == data->finalInstruction) {
        return true;
    }
    else {
        tempInstruction = data->currentInstruction + 4;
        while(blacklist.search(tempInstruction)) {
            if(tempInstruction == data->finalInstruction) {
                return true;
            }
            tempInstruction += 4;
            data->numInstrExec++;
        }
        *nextInstruction = tempInstruction;
        return false;
    }
}
