#ifndef IPC_CREATE_PROCESS_H
#define IPC_CREATE_PROCESS_H

#include "CreateProcess.h"

class IPCCreateProcess : public CreateProcess {
public:
    IPCCreateProcess(const std::string& command) : CreateProcess(command) {}

    void pipesIPC() {
        // Add pipe IPC logic here
        std::cout << "Using pipes for IPC" << std::endl;
    }

    void shared_memoryIPC() {
        // Add shared memory IPC logic here
        std::cout << "Using shared memory for IPC" << std::endl;
    }

    void mmapIPC() {
        // Add memory-mapped file IPC logic here
        std::cout << "Using memory-mapped files for IPC" << std::endl;
    }

    void socketIPC() {
        // Add socket IPC logic here
        std::cout << "Using sockets for IPC" << std::endl;
    }
};

#endif // IPC_CREATE_PROCESS_H
