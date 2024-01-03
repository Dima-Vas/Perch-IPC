// Tests and lib using example
#include <PIPC/api/SharedMemory.h>
#include <PIPC/api/ProcessCreation.h>
#include <iostream>
#include <string>
#include <cstring>

int main() {
    // Shared memory size and name
    const size_t sharedMemorySize = 1;
    const std::string sharedMemoryName = "/my_shared_memory";
    ProcessCreation pc;

    // Create shared memory
    SharedMemory<long> shmem(sharedMemoryName, sharedMemorySize);
    shmem.write(0l, 0);

    int pid = fork();

    if (pid < 0) {
        std::cerr << "Bad fork : " << strerror(errno) << std::endl;
    } else if (pid == 0) {
        // Child process
        int i = 0;
        while (i < 10) {
            auto result = shmem.compare_and_swap(0l, shmem.read(0) - 1l);
            std::cout << result << std::endl;
            ++i;
        }
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        int i = 0;
        while (i < 10) {
            auto result = shmem.compare_and_swap(0l, shmem.read(0) + 1l);
            std::cout << result << std::endl;
            ++i;
        }
        Process child;
        child.setPID(pid);
        pc.wait_for_exit(child);
    }
    return 0;
}
