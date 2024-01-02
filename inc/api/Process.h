#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>


class Process {
public:
    Process() {}
    Process(const char* path, const char* const* args = nullptr) : executablePath(path) {
        arguments.push_back(executablePath);
        if (args != nullptr) {
            while (*args != nullptr) {
                arguments.push_back(*args);
                args++;
            }
        }
    }
    ~Process() {};
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