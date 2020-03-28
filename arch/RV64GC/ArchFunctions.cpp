#include "ArchFunctions.h"
#include "Blacklist.h"
#include "Analyser.h"
#include <RV-disas/riscv-disas.h>
#include <string.h>
#include <string>
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
    bool isValid = false;
    if(scanner->currentInstructionSize == instructionSizeUC) {
        const uint8_t* firstInstructionByte = (uint8_t*)scanner->instructionPointer;
        uint64_t firstByteOffset = 0;
        uint64_t size = scanner->currentInstructionSize;
        isValid = cs_disasm_iter(scanner->capstoneHandle, &firstInstructionByte, &size, &firstByteOffset, scanner->capstoneInstruction);
    }
    else if(scanner->currentInstructionSize == instructionSize) {
        isValid = disasm_inst(rv64, 0x1000, scanner->currentInstruction);
    }
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



void basicAnalysisAndSizeCheckRiscv(Scanner* data) {
    int signo = data->lastSigno;
    siginfo_t* info = data->lastInfo;
    ucontext_t* context = (ucontext_t*)(data->lastContext);
    reg_t pc = context->uc_mcontext.__gregs[REG_PC];

    if(signo == SIGSEGV && pc == (reg_t)data->instructionPointer && (reg_t)info->si_addr == ((reg_t)data->instructionPointer + instructionSize)) {
        data->sizeError = true;
        return;
    }
    basicAnalysis(data);
}

void riscvLengthLogAnalysis(Scanner* data) {
    int signo = data->lastSigno;
    siginfo_t* info = data->lastInfo;
    ucontext_t* context = (ucontext_t*)(data->lastContext);

    reg_t pc = context->uc_mcontext.__gregs[REG_PC];
    int size;
    if(signo == SIGSEGV && pc == (reg_t)data->instructionPointer && (reg_t)info->si_addr == ((reg_t)data->instructionPointer + instructionSize)) {
        size = instructionSizeUC;
    }
    else {
        size = instructionSize;
    }

    std::string output = std::to_string(data->currentInstruction)+ " " + std::to_string(signo) + " " + std::to_string(info->si_code) +
     " " + std::to_string((reg_t)data->instructionPointer) +  " " + std::to_string(pc) + " "
     + std::to_string((reg_t)info->si_addr) +  " " + std::to_string(size) + "\n";
    write(data->outputFD, output.c_str(), output.size());
}

bool riscvcOnlyFetch(Scanner* data, instr_t* nextInstruction) {
    instr_t tempInstruction;
    if(data->currentInstruction >= data->finalInstruction) {
        return true;
    }
    else {
        tempInstruction = data->currentInstruction + 1;
        if((tempInstruction & 0b11) == 0b11) {
            tempInstruction++;
            data->numInstrExec++;
        }
        while(blacklist.search(tempInstruction)) {
            if(tempInstruction >= data->finalInstruction) {
                return true;
            }
            tempInstruction+= 1;
            data->numInstrExec++;
            if((tempInstruction & 0b11) == 0b11) {
                tempInstruction++;
                data->numInstrExec++;
            }
        }
        *nextInstruction = tempInstruction;
        return false;
    }
}


instr_t construct(instr_t part[2]) {
    return part[0] | part[1] << (instructionSize * 8);
}

bool vleExhaustive(Scanner* data, instr_t* nextInstruction) {
    instr_t tempInstruction;
    bool found = false;
    if(data->sizeError) {
        data->numInstrExec++;
        data->sizeError = false;
        data->instructionPointer = (uint8_t*)(data->instructionPage) + pageSize - instructionSizeUC;
        data->currentInstructionSize = instructionSizeUC;
        *nextInstruction = data->currentInstruction;
        return false;
    }

    //deconstruct current instruction
    size_t part2Size = instructionSizeUC - instructionSize;
    instr_t partMask[2] = {((instr_t)-1 >> (sizeof(instr_t) - instructionSize) * 8), (((instr_t)-1 >> (sizeof(instr_t) - part2Size) * 8)) << (instructionSize * 8)};
    instr_t part[2] = {data->currentInstruction & partMask[0], (data->currentInstruction & partMask[1]) >> (instructionSize * 8)};

    tempInstruction = data->currentInstruction;
    if(data->currentInstructionSize == instructionSizeUC) {
        while(!found) {
            if(tempInstruction == data->finalInstruction) {
                return true;
            }
            if(part[1] == partMask[0]) {
                writeStdPage(data->instructionPage);
                data->instructionPointer = (uint8_t*)(data->instructionPage) + pageSize - instructionSize;
                data->currentInstructionSize = instructionSize;
                break;
            }
            part[1] = part[1] + 1;
            tempInstruction = construct(part);
            found = !blacklist.search(tempInstruction);
            data->numInstrExec++;
        }
        if(found) {
            *nextInstruction = tempInstruction;
            return false;
        }
    }

    part[1] = 0;
    while(!found) {
        if((tempInstruction & partMask[0]) == (data->finalInstruction & partMask[0])) {
            return true;
        }
        part[0] += 1;
        tempInstruction = construct(part);
        found = !blacklist.search(tempInstruction);
        data->numInstrExec++;
    }
    if(found) {
        *nextInstruction = tempInstruction;
        return false;
    }
    return true;
}
