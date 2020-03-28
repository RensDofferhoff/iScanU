#pragma once

#include <unordered_map>
#include "Scanner.h"

enum class Method { MEMCAGE, PTRACE, KERNELMOD };
extern Method method;
extern uint8_t* stdPage;
extern std::unordered_map<pid_t, Scanner*> threadDataMap;
extern int managerFD;
extern int performanceLogFD;
extern int performanceLogSlowFactor;
extern int performanceLogCount;
extern bool checkForHang;
Scanner* getThreadData(pid_t tid);
void removeThread(pid_t tid);

class ScannerManager {
public:
    ScannerManager(int _numThreads) : numThreads(_numThreads) {};
    virtual void runScanners() = 0;
    void setAffinity(int numCores);
protected:
    int numThreads;
};
