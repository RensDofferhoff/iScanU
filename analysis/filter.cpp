#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <cstdint>
#include <vector>
#include <functional>
#include <set>
#include <cmath>
#include <algorithm>

typedef uint32_t instr_t;
using namespace std;

uint32_t readInt(string s) {
    return stoul(s, nullptr, 10);
}

struct InstructionMask {
    InstructionMask(std::initializer_list<instr_t> l) : valueMask(*l.begin()), dontCareMask(*(l.begin() + 1)) {};
    instr_t valueMask;
    instr_t dontCareMask;
};



class LowMemoryBlacklist {
public:
    LowMemoryBlacklist(std::initializer_list<InstructionMask> startList);
    ~LowMemoryBlacklist();

    bool search(instr_t k);
    void add(instr_t k);
    void add(InstructionMask k);

private:
    //NOTE: The dontCareMasks in the searchSet have been inverted and are thus used as do care masks
    std::set<std::pair<instr_t, instr_t>> searchSet;

};

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


LowMemoryBlacklist blacklist = {
    { 0b10001000101000000111110000000000, 0b11000000010111111000001111111111 },
    { 0b00001000001000000111110000000000, 0b01000000010111111000001111111111 },
    { 0b01111000001000000000000000000000, 0b11000000110111110111001111111111 },
    { 0b00001000110000000000000000000000, 0b11000000000111110111111111111111 },
    { 0b01111111000000001101000000000000, 0b00000000111111110000101111111111 },
    { 0b00101111000000001101000000000000, 0b01000000111111110000101111111111 },
    { 0b01111110000000001000010000000000, 0b00000000110111110000001111111111 },
    { 0b00101110000000001000010000000000, 0b01000000110111110000001111111111 },
    { 0b01111111000000001111000000000000, 0b00000000111111110000101111111111 },
    { 0b00101111000000001111000000000000, 0b01000000111111110000101111111111 },
    { 0b01111110000000001000110000000000, 0b00000000110111110000001111111111 },
    { 0b00101110000000001000110000000000, 0b01000000110111110000001111111111 },
    { 0b00111000001000000000000000011111, 0b11000000010111110111001111100000 },
    { 0b00001000100000000000000000000000, 0b11000000000111110111111111111111 },
    { 0b00111000001000001000001111111111, 0b11000000110111110000001111111111 }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Need the following arguments: file with insns" << std::endl;
    }
    string line;
    uint32_t insn;
    ifstream insns;
    insns.open(argv[1], ios_base::in);

    getline(insns, line);
    insn = readInt(line);
    while (true) {
        if (!blacklist.search(insn)) {
            printf("%u\n", insn);
        }

        getline(insns, line);
        if (insns.eof()) {
            break;
        }
        insn = readInt(line);
    }
}
