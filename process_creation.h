//
// Created by Admin on 30.10.2023.
//

#ifndef MYRLS_PROCESS_CREATION_H_H
#define MYRLS_PROCESS_CREATION_H_H

#include <string>
#include <vector>

#include <string>
#include <vector>

class ProcessCreation {
public:
    ProcessCreation() = default;

    // Spawns a child process with the given command and arguments.
    static static static int spawn(const std::string& command, const std::vector<std::string>& arguments);

    // Launches a child process with the given command and arguments and returns immediately.
    static int launch(const std::string& command, const std::vector<std::string>& arguments);

    // Waits for the child process to complete and returns its exit code.
    static int wait_for_exit();

    // Kills the child process and returns its exit code.
    int kill();
};

#endif //MYRLS_PROCESS_CREATION_H_H
