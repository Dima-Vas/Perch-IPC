
#ifndef LINUX_PROC_CREATION
#define LINUX_PROC_CREATION
#include "../api/Process.h"
#include <semaphore.h>
int linuxLaunch(Process&);
int linuxKill(Process&, int killSig);
int linuxWaitForExit(Process&);
int* linuxPipeRedirectOutput(Process& out_process, Process& in_process, const char* sem_name);
#endif
