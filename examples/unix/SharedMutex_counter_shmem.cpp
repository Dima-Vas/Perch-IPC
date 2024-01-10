/*
    SharedMutex used for IPC synchronization like mutex for threads.
*/

#include <PIPC/api/SharedMutex.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <iostream>
#include <fcntl.h>

int main() {
    SharedMutex sh_mut("shared_mutex");
    std::string name = "int";
    #ifdef __FreeBSD__
        name = "/usr/local/PIPC/" + name;
    #endif
    int shm_fd = shm_open(name.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
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
    return 0;
}
