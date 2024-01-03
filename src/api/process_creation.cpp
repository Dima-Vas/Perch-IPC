#include "ProcessCreation.h"
#include "Pipe.h"

int ProcessCreation::launch(Process& process) {
    int launchedPID = -1;
    #ifdef __linux__
        launchedPID = linuxLaunch(process);
    #endif
    return launchedPID;
}

int ProcessCreation::wait_for_exit(Process& process) {
    int status = -1;
    #ifdef __linux__
        status = linuxWaitForExit(process);
    #endif
    return status;
}

int ProcessCreation::kill(Process& process, int killsig) {
    #ifdef __linux__
        return linuxKill(process, killsig);
    #endif
    return 0;
}

Pipe ProcessCreation::pipe_IPC(Process& out_process, Process& in_process) {
    Pipe to_return{};
    to_return.connect(out_process, in_process);
    return to_return;
}
