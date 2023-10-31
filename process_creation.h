//
// Created by Admin on 30.10.2023.
//

#ifndef MYRLS_PROCESS_CREATION_H
#define MYRLS_PROCESS_CREATION_H

#include <string>
#include <vector>
#include "Process.h"


class ProcessCreation {
public:
    ProcessCreation() = default;

    // Spawns a child process with the given command and arguments.
    int spawn(Process process);

    // Launches a child process with the given command and arguments and returns immediately.
    int launch(Process process);

    // Waits for the child process to complete and returns its exit code.
    int wait_for_exit(Process process);

    // Kills the child process and returns its exit code.
    int kill(Process process);

    int redirect_output(Process out_process, Process in_process);
};

#endif //MYRLS_PROCESS_CREATION_H
