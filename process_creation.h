#ifndef MYRLS_PROCESS_CREATION_H
#define MYRLS_PROCESS_CREATION_H

#include <string>
#include <vector>
#include "Process.h"


class ProcessCreation {
public:
    ProcessCreation() = default;

    int spawn(Process process);

    int launch(Process process);

    int wait_for_exit(Process process);

    int kill(Process process);

    int redirect_output(Process out_process, Process in_process);
};

#endif //MYRLS_PROCESS_CREATION_H
