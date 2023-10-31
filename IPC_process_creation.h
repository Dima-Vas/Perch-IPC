#ifndef IPC_CREATE_PROCESS_H
#define IPC_CREATE_PROCESS_H

#include "process_creation.h"
#include "Pipe.h"
#include "Shared_memory.h"

class IPC_process_creation : public ProcessCreation {
public:
    int pipesIPC(Process process1, Process process2, Pipe pipe);

    int shared_memoryIPC(Process process1, Process process2, Shared_memory sharedMemory);

//    int mmapIPC(Process process1, Process process2);
//
//    int socketIPC(Process process1, Process process2);
};

#endif // IPC_CREATE_PROCESS_H
