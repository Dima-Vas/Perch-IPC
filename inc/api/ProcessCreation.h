#ifndef PROCESS_CREATION_H
#define PROCESS_CREATION_H

#include <string>
#include <vector>
#include "Process.h"
#include "Pipe.h"


class ProcessCreation {
public:
    ProcessCreation() = default;

    int launch(Process& process);

    int wait_for_exit(Process& process);

    int kill(Process& process, int killsig);

    Pipe pipe_IPC(Process& out_process, Process& in_process);
};

#endif //PROCESS_CREATION_H
