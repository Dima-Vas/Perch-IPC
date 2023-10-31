#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

class Process {
public:
    Process(const std::string& path) : executablePath(path) {}
    Process(const char* path, const char* const* args = nullptr) : executablePath(path) {
        if (args != nullptr) {
            while (*args != nullptr) {
                arguments.push_back(*args);
                args++;
            }
        }
    }
    const std::string& getPath() const {
        return executablePath;
    }
    const std::vector<std::string>& getArguments() const {
        return arguments;
    }
private:
    std::string executablePath;
    std::vector<std::string> arguments;
};

#endif // PROCESS_H