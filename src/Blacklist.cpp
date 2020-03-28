#include "Blacklist.h"


Blacklist::Blacklist(std::initializer_list<InstructionMask> startList) {
    //make searchSet more sparse by setting lower max load factor for better performance
    //burns memory like there is no tomorrow sadly
    searchSet.max_load_factor(0.5);
    for(const InstructionMask& mask : startList) {
        add(mask);
    }
}

Blacklist::~Blacklist() {

}

bool Blacklist::search(instr_t k) {
    auto rangeIt = searchSet.find(k);
    if(rangeIt == searchSet.end()) {
        return false;
    }
    return true;

}

void Blacklist::add(instr_t k){
    searchSet.insert(k);
}

void Blacklist::add(InstructionMask m){
    instr_t numDontCares = 0;
    instr_t mask = 0b1;
    for(instr_t i = 0; i < sizeof(instr_t) * 8; ++i) {
        if(m.dontCareMask & (mask << i)) {
            numDontCares++;
        }
    }

    instr_t instruction;
    instr_t k;
    for(instr_t i = 0; i < (instr_t)(1 << numDontCares); ++i) {
        k = 0;
        instruction = m.valueMask & ~m.dontCareMask;
        //fill dont care value in dont care fields
        for(instr_t j = 0; j < numDontCares; ++j) {
            for(; k < sizeof(instr_t) * 8; ++k) {
                if(m.dontCareMask & (mask << k)) {
                    instruction |= (i & (mask << j)) << (k - j);
                    ++k;
                    break;
                }
            }
        }
        searchSet.insert(instruction);
    }
}




LowMemoryBlacklist::LowMemoryBlacklist(std::initializer_list<InstructionMask> startList) {
    for(const InstructionMask& mask : startList) {
        add(mask);
    }
}

LowMemoryBlacklist::~LowMemoryBlacklist() {

}

bool LowMemoryBlacklist::search(instr_t k) {
    instr_t processedRefInstruction, careMask;
    for(const auto& entry : searchSet) {
        processedRefInstruction = entry.first;
        careMask = entry.second;
        if ((k & careMask) == processedRefInstruction) {
            return true;
        }
    }
    return false;
}

void LowMemoryBlacklist::add(instr_t k) {
    const instr_t EMPTY_DONT_CARE_MASK = 0;
    InstructionMask im = { k, EMPTY_DONT_CARE_MASK };
    add(im);
}

void LowMemoryBlacklist::add(InstructionMask m) {
    instr_t careMask = ~m.dontCareMask;
    instr_t processedRefInstruction = m.valueMask & careMask;
    auto entry = std::make_pair(processedRefInstruction, careMask);
    searchSet.insert(entry);
}
