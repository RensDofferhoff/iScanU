#pragma once

#include "ScannerManager.h"

class ScannerManagerMC : public ScannerManager {
public:
    ScannerManagerMC(int numThreads, uint64_t first, uint64_t last);
    ~ScannerManagerMC();
    void runScanners();
private:
    stack_t altStack;
    
    void initStdPage();
    void registerHandlers();
    void localScannerInit(Scanner* data);
};
