#pragma once

#include <unordered_set>
#include <set>
#include <list>
#include "ArchProperties.h"
#include <initializer_list>

struct InstructionMask {
    InstructionMask(std::initializer_list<instr_t> l) : valueMask(*l.begin()), dontCareMask(*(l.begin() + 1)) {};
    instr_t valueMask;
    instr_t dontCareMask;
};

class Blacklist {

public:
    Blacklist(std::initializer_list<InstructionMask> startList);
    ~Blacklist();

    bool search(instr_t k);
    void add(instr_t k);
    void add(InstructionMask k);

private:
    std::unordered_set<instr_t> searchSet;

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
