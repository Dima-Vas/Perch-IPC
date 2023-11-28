#ifndef MYRLS_PROCESS_CREATION_H
#define MYRLS_PROCESS_CREATION_H

#if _WIN32
#endif

#ifdef __linux__
#endif

#include <string>
#include <vector>
#include "Process.h"
#include "Pipe.h"


class ProcessCreation {
public:
    ProcessCreation() = default;

    int launch(Process& process);

    int wait_for_exit(Process& process);

    int kill(Process& process);

    Pipe pipe_IPC(Process& out_process, Process& in_process);
};

#endif //MYRLS_PROCESS_CREATION_H
