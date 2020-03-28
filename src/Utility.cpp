#include <string>
#include <assert.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <future>
#include "ScannerManager.h"

bool debugMode = false;

int openCriticalOutputFile(std::string path) {
    auto temp = umask(0);
    int fd = open(path.c_str(), O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    umask(temp);
    if(fd == -1) {
        printf("could not open output file %s\n", path.c_str());
        printf("%s\n", strerror(errno));
        exit(-1);
    }
    return fd;
}

void createCriticalOutputDir() {
    auto temp = umask(0);
    mkdir("results", S_IRWXU | S_IRWXG | S_IRWXO);
    umask(temp);
}



void writeTimestamp(int fd) {
    std::string output = "time: " + std::to_string(time(NULL)) + "\n";
    write(fd, output.c_str(), output.size());
}

void stopWorker(Scanner* data) {
    data->isStopped = true;
    writeTimestamp(data->outputFD);
    std::string output =  "======= end of run =======\n";
    write(data->outputFD, output.c_str(), output.size());
    fsync(data->outputFD);
    std::promise<void>().get_future().wait(); //Waits forever, is cleaned up in alarm handler
}
