#pragma once
#include "Scanner.h"

//Function pointer linked to selected analysis function called in faultHandler
extern void (*analyse)(Scanner*);

//Determines if the instruction is hidden based on disassembler and signal number analysis
void basicAnalysis(Scanner* data);

//Gives more information on every scanned instruction
void insnAnalysis(Scanner* data);
