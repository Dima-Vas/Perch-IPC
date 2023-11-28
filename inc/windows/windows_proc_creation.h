#ifndef WINDOWS_PROC_CREATION
#define WINDOWS_PROC_CREATION
#include <signal.h>
#include "Process.h"
#include <semaphore.h>
int windowsLaunch(Process&);
int windowsKill(Process&, bool killSig = SIGTERM);
int windowsWaitForExit(Process&);
int* windowsPipeRedirectOutput(Process& out_process, Process& in_process, const char* sem_name);
#endif
