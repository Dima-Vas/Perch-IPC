/**
 * @file Process.h
 * @brief This file contains the declaration of Process class.
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

/**
 * @class Process
 * @brief A container class representing an entity of the process.
 *
 * An entity for the processes you initialize yourself in your code. Process class acts as a container,
 * saving executable path, arguments and process id of it.
 */
class Process {
public:

    /**
     * @brief Default constructor for Process class.
     */
    Process() {}

    /**
     * @brief Basic constructor for Process class.
     * @param path path for an executable. Supports PATH.
     * @param args arguments passed to this executable.
     */
    Process(const char* path, const char* const* args = nullptr) : executablePath(path) {
        arguments.push_back(executablePath);
        if (args != nullptr) {
            while (*args != nullptr) {
                arguments.push_back(*args);
                args++;
            }
        }
    }

    ~Process() = default;

    /**
     * @brief Push an additional argument into the arguments vector of a Process entity.
     * @param arg additional argument to push.
     */
    void addArgument(const std::string& arg) {
        arguments.push_back(arg);
    }

    const std::string& getPath() const {
        return executablePath;
    }

    const std::vector<std::string>& getArguments() const {
        return arguments;
    }

    void setPID(int pidToSet) {
        pid = pidToSet;
    }

    int getPID() {
        return pid;
    }

private:
    std::string executablePath;
    std::vector<std::string> arguments;
    int pid;
};

#endif // PROCESS_H