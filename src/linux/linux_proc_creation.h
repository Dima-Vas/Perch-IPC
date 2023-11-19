
#ifndef LINUX_PROC_CREATION
#define LINUX_PROC_CREATION
#include <signal.h>
int linuxLaunch(Process&);
int linuxKill(Process&, bool killSig = SIGTERM);
int linuxWaitForExit(Process&);
int* linuxPipeRedirectOutput(Process& out_process, Process& in_process);
#endif