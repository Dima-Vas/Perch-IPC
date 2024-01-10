/*
    SharedMutex used for IPC synchronization like mutex for threads.
*/

#include <PIPC/api/SharedMutex.h>
#include <PIPC/api/SharedMemory.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <iostream>
#include <fcntl.h>

int main() {
    SharedMutex sh_mut("shared_mutex");
    #ifdef __FreeBSD__
        name = "/usr/local/PIPC/" + name;
    #endif
    SharedMemory<int> sh_mem{"random_name", 1};

    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Error in fork()" << std::endl;
        return 1;
    }

    if (pid == 0) {
        while (1) {
            sh_mut.lock();
            int c_read = sh_mem.read(0);
            if (c_read >= 5) {
                sh_mut.unlock();
                break;
            }
            std::cout << "Child process: Before Increment Counter = " << c_read << std::endl;
            ++c_read;
            sh_mem.write(c_read, 0);
            std::cout << "Child process: After Increment Counter = " << c_read << std::endl;
            sh_mut.unlock();
        }
        exit(0);
    } else {
        while (true) {
            sh_mut.lock();
            int c_read = sh_mem.read(0);
            if (c_read >= 5) {
                sh_mut.unlock();
                break;
            }
            std::cout << "Parent Process: Before Increment Counter = " << c_read << std::endl;
            ++c_read;
            sh_mem.write(c_read, 0);
            std::cout << "Parent Process: After Increment Counter = " << c_read << std::endl;
            sh_mut.unlock();
        }
        waitpid(pid, NULL, 0);
    }

    return 0;
}
