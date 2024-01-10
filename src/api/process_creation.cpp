#include "ProcessCreation.h"
#include "Pipe.h"

/**
 * @brief Launches the process represented by Process entity and sets pid of the passed process.
 * @param process The process to launch.
 * @return Id of the newly created process upon success; else -1.
 */
int ProcessCreation::launch(Process& process) {
    int launchedPID = -1;
    #if defined(__linux__) || defined(__FreeBSD__)
        launchedPID = linuxLaunch(process);
    #endif
    #if defined(_WIN32)
        launchedPID = windowsLaunch(process);
    #endif
    return launchedPID;
}

/**
 * @brief Spins the process that casted wait_for_exit until the passed process is finished.
 * @param process The process to wait for exit.
 * @return Pid upon success, 0 if process has not changed its state yet and -1 if fails.
 */
int ProcessCreation::wait_for_exit(Process& process) {
    int status = -1;
    #if defined(__linux__) || defined(__FreeBSD__)
        status = linuxWaitForExit(process);
    #endif
    #if defined(_WIN32)
        status = windowsWaitForExit(process);
    #endif
    return status;
}

/**
 * @brief Send signal killsig to a process.
 * @param process The process to wait for exit.
 * @param killsig The signal number to send.
 * @return 0 upon successful send; else 1.
 */
int ProcessCreation::kill(Process& process, int killsig) {
    #if defined(__linux__) || defined(__FreeBSD__)
        return linuxKill(process, killsig);
    #endif
    #if defined(_WIN32)
        return windowsKill(process, killsig);
    #endif
    return 0;
}

/**
 * @brief Connects two Processes with pipe and sets pipe to be ready for transfering.
 * @param out_process The process to generate output.
 * @param in_process The process to receive input.
 * @return New instance of Pipe containing connected processes.
 */
Pipe ProcessCreation::pipe_IPC(Process& out_process, Process& in_process) {
    Pipe to_return{};
    to_return.connect(out_process, in_process);
    return to_return;
}
