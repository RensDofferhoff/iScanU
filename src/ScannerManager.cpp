#include "ScannerManager.h"
#include <string.h>
#include <sched.h>

uint8_t* stdPage;
std::unordered_map<pid_t, Scanner*> threadDataMap;
int managerFD;
int performanceLogFD;
int performanceLogSlowFactor;
int performanceLogCount;
bool checkForHang;
Method method;

Scanner* getThreadData(pid_t tid) {
    return threadDataMap[tid];
}

void removeThread(pid_t tid) {
    threadDataMap.erase(tid);
}


void ScannerManager::setAffinity(int numCores) {
    cpu_set_t* cpuSet = CPU_ALLOC(numCores);
    size_t cpuSetSize =  CPU_ALLOC_SIZE(numCores);

    int i = 0;
    for(auto thread : threadDataMap) {
        CPU_ZERO_S(cpuSetSize, cpuSet);
        CPU_SET_S(i, cpuSetSize, cpuSet);
        if(sched_setaffinity(thread.first, cpuSetSize, cpuSet) == -1) {
            printf("Failed to set affinity\n");
            printf("%s\n", strerror(errno));
        }
        i = (i + 1) % numCores;
    }
    free(cpuSet);
}
