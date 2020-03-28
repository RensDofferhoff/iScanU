#pragma once

#include "ScannerManager.h"

class ScannerManagerPtrace : public ScannerManager {
public:
    ScannerManagerPtrace(int numThreads, uint64_t first, uint64_t last);
    ~ScannerManagerPtrace();

    void runScanners();
private:
    void localScannerInit(Scanner* data);
    void registerHandlers();
};
