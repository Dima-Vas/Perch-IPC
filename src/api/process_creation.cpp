#include "process_creation.h"
#include "linux_proc_creation.h"
#include "windows_proc_creation.h"
#include "Pipe.h"

int ProcessCreation::launch(Process& process) {
    int launchedPID = -1;
    #ifdef __linux__
        launchedPID = linuxLaunch(process);
    #endif
    #ifdef _WIN32
        launchedPID = windowsLaunch(process);
    #endif
    return launchedPID;
}

int ProcessCreation::wait_for_exit(Process& process) {
    int status = -1;
    #ifdef __linux__
        status = linuxWaitForExit(process);
    #endif
    #ifdef _WIN32
        status = windowsWaitForExit(process);
    #endif
    return status;
}

int ProcessCreation::kill(Process& process) {
    #ifdef __linux__
        return linuxKill(process);
    #endif
    #ifdef _WIN32
        return windowsKill(process);
    #endif
    return 0;
}

Pipe ProcessCreation::pipe_IPC(Process& out_process, Process& in_process) {
    Pipe to_return{};
    to_return.connect(out_process, in_process);
    return to_return;
}
