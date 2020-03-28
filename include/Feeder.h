#pragma once
#include "Scanner.h"
#include "ArchProperties.h"

//must return true if finished otherwise returns false
//writes next instruction at nextInstruction. Called in faultHandler
extern bool (*fetchInstruction)(Scanner*, instr_t*);

//Next instruction is current + 1, unless blacklisted
bool exhaustive(Scanner* data, instr_t* nextInstruction);
