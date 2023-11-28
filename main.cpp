// Tests and lib using example
#include "process_creation.h"
#include "linux_proc_creation.h"
#include "IPC_process_creation.h"
#include "Pipe.h"
#include <iostream>
#include "SharedMutex.h"

#ifdef __linux__
#include <sys/wait.h>
#endif


// int main(int argc, char* argv[]) {
//     Process proc1("ls");
//     Process proc2("echo");
//     ProcessCreation pc;
//     Pipe pipe = pc.pipe_IPC(proc2, proc1);
//     Process proc3("pwd");
//     pipe.transfer(); // this simply increments the semaphore inside of Pipe and checks for errors
//     pc.wait_for_exit(proc1);
//     pc.wait_for_exit(proc2);
//     pc.launch(proc3);
//     return 0;
// }

int main() {

#if _WIN32
#endif

#ifdef __linux__
#endif


#ifdef __linux__

    SharedMutex sh_mut("shared_mutex");
    int shm_fd = shm_open("int", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shm_fd, sizeof(int));
    int* counter = (int*)mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Error in fork()" << std::endl;
        return 1;
    }

    if (pid == 0) {
        while (*counter < 5) {
            sh_mut.lock();
            if (*counter >= 5) {
                break;
            }
            std::cout << "Child (Reader) Process: Before Increment Counter = " << *counter << std::endl;
            *counter = *counter + 1;
            std::cout << "Child (Reader) Process: After Increment Counter = " << *counter << std::endl;
            sh_mut.unlock();
        }
        exit(0);
    } else {
        while (*counter < 5) {
            sh_mut.lock();
            if (*counter >= 5) {
                break;
            }
            std::cout << "Parent (Writer) Process: Before Increment Counter = " << *counter << std::endl;
            *counter = *counter + 1;
            std::cout << "Parent (Writer) Process: After Increment Counter = " << *counter << std::endl;
            sh_mut.unlock();
        }
        waitpid(pid, NULL, 0);
    }

    ftruncate(shm_fd, 0);
    shm_unlink("int");
#endif

    return 0;
}
