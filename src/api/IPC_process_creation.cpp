#include "IPC_process_creation.h"

#if _WIN32
#endif

#ifdef __linux__
#endif

template <typename T>
SharedMemory<T> IPC_process_creation :: shared_memoryIPC(std::string memoryName, size_t memorySize){
    SharedMemory<T> shm{memoryName, memorySize, access};
    return shm;
};

//int IPC_process_creation :: mmapIPC(Process process1, Process process2){
//    return 0;
//};
//
int IPC_process_creation :: socketIPC(Process process1, Process process2){
   return 0;
};