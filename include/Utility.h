#pragma once

extern bool debugMode;

int openCriticalOutputFile(std::string path);
void createCriticalOutputDir();
void writeTimestamp(int fd);
void stopWorker(Scanner* data);
