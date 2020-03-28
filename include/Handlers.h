#pragma once

#include <signal.h>

//Executes main scanning loop in the memcage method
void faultHandler(int signo, siginfo_t* info, void* context);

//Handler executes manager tasks: logs performance, checks for hangs
void alarmHandler(int signo, siginfo_t* info, void* context);

//Stores good state for scanner loop and starts scanner loop
void entryHandler(int signo, siginfo_t* info, void* context);

//Executed when scanner gets stuck, forces move to next instruction
void hangHandler(int signo, siginfo_t* info, void* context);


//Empty functions
void startHandlerPtrace(int signo, siginfo_t* info, void* context);

void hangHandlerPtrace(int signo, siginfo_t* info, void* context);
