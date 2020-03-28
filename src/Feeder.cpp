//Contains general fetch stages that can be set in main.cpp

#include "Feeder.h"
#include "Blacklist.h"
#include "ArchProperties.h"

bool (*fetchInstruction)(Scanner*, instr_t*);

bool exhaustive(Scanner* data, instr_t* nextInstruction) {
    instr_t tempInstruction;
    if(data->currentInstruction == data->finalInstruction) {
        return true;
    }
    else {
        tempInstruction = data->currentInstruction + 1;
        while(blacklist.search(tempInstruction)) {
            if(tempInstruction == data->finalInstruction) {
                return true;
            }
            tempInstruction++;
            data->numInstrExec++;
        }
        *nextInstruction = tempInstruction;
        return false;
    }
}
