#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <cstdint>
#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>

using namespace std;


void printAsBinary(uint32_t value) {
    char s[33];
    s[32] = 0;
    for (int i = 31; i >= 0; --i) {
        s[31 - i] = (value & (1 << i)) ? '1' : '0';
    }
    printf("%s\n", s);
}

void printAsMaskedBinary(uint32_t value, uint32_t mask) {
    char s[33];
    s[32] = 0;
    for (int i = 31; i >= 0; --i) {
        s[31 - i] = (value & (1 << i)) ? '1' : '0';
        if (mask & (1 << i)) {
            s[31 - i] = 'x';
        }
    }
    printf("%s\n", s);
}

bool isPower2(uint32_t value) {
    return value != 0 && ((value & (value - 1)) == 0);
}

struct MaskedInstruction {
    MaskedInstruction() {};
    MaskedInstruction(uint32_t instr, uint32_t mask) : instr(instr), mask(mask) {};
    uint32_t instr;
    uint32_t mask;
};

bool inSameRange(MaskedInstruction val, MaskedInstruction other, uint32_t delta) {
    return val.instr + delta == other.instr && val.mask == other.mask;
}

int main(int argc, char* argv[]) {
    ifstream insns;
    insns.open("fpOutput.txt", ios_base::in);
    vector<MaskedInstruction> instructions;
    vector<MaskedInstruction> output;
    string line;
    uint32_t instruction, mask;
    while(true) {
        getline(insns, line);
        if (insns.eof())
            break;
        size_t split = line.find_first_of(" ");
        string in = line.substr(0, split);
        string ma = line.substr(split + 1);
        instruction = stoul(in, nullptr, 10);
        mask = stoul(ma, nullptr, 10);
        instruction = instruction & ~mask;
        MaskedInstruction mi(instruction, mask);
        instructions.push_back(mi);
    }

    int begin, end, diff, variableBits, deltaBitShift;
    int delta = instructions[1].instr - instructions[0].instr;
    for (int i = 0; i < instructions.size(); ++i) {
        begin = i;
        if (!isPower2(delta)) {
            output.push_back(instructions[i]);
            if (i + 2 < instructions.size()) {
                delta = instructions[i + 2].instr - instructions[i + 1].instr;
            }
            continue;
        }
        while (i + 1 < instructions.size() && inSameRange(instructions[i], instructions[i + 1], delta)) {
            ++i;
        }
        end = i;
        diff = end - begin;
        variableBits = log2(diff);
        if (!isPower2(diff + 1)) {
            variableBits -= 1;
            i -= diff + 1 - pow(2, variableBits);
        }
        deltaBitShift = log2(delta) - 1;
        mask = 0;
        for (int k = 0; k < variableBits; ++k) {
            mask |= 1 << (k + deltaBitShift);
        }
        uint32_t newMask = instructions[begin].mask | mask;
        MaskedInstruction mi(instructions[begin].instr, newMask);
        output.push_back(mi);
        if (i + 2 < instructions.size()) {
            delta = instructions[i + 2].instr - instructions[i + 1].instr;
        }
    }

    for (const auto& instruction : output) {
        //cout << instruction.instr << " " << instruction.mask << std::endl;
        printAsMaskedBinary(instruction.instr, instruction.mask);
        //printAsBinary(instruction.instr);
        //printAsBinary(instruction.mask);
        //cout << std::endl;
    }
}
