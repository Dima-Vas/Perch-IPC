#ifndef WIN32_PROC_CREATION
#define WIN32_PROC_CREATION
#include <signal.h>
#include "../api/Process.h"
#include <semaphore.h>
int windowsLaunch(Process&);
int windowsKill(Process&, int killSig);
int windowsWaitForExit(Process&);
int* windowsPipeRedirectOutput(Process& out_process, Process& in_process, const char* sem_name);
#endif
