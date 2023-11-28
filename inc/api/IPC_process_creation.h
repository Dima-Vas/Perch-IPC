#ifndef IPC_CREATE_PROCESS_H
#define IPC_CREATE_PROCESS_H

#include "process_creation.h"
#include "Pipe.h"
#include "SharedMemory.h"

#if _WIN32
#endif

#ifdef __linux__
#endif

class IPC_process_creation : public ProcessCreation {
public:
    template <typename T>
    SharedMemory<T> shared_memoryIPC(std::string memoryName, size_t memorySize);

//    int mmapIPC(Process process1, Process process2);
//
   int socketIPC(Process process1, Process process2);

};

#endif // IPC_CREATE_PROCESS_H
