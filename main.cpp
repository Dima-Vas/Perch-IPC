// Tests and lib using example
#include "process_creation.h"
#include "linux_proc_creation.h"
#include "IPC_process_creation.h"
#include "Pipe.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Process proc1("ls");
    Process proc2("echo");
    ProcessCreation pc;
    Pipe pipe = pc.pipe_IPC(proc2, proc1);
    Process proc3("pwd");
    pipe.transfer(); // this simply increments the semaphore inside of Pipe and checks for errors
    pc.wait_for_exit(proc1);
    pc.wait_for_exit(proc2);
    pc.launch(proc3);
    return 0;
}

// int main(int argc, char* argv[]) {
//     // Process proc1();
//     IPC_process_creation ipc;
//     SharedMemory shm = ipc.shared_memoryIPC("shared", 1024);
//     return 0;
// }
