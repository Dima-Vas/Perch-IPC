
#ifndef SHARED_MUTEX_H
#define SHARED_MUTEX_H
#include <mutex>
#include <iostream>
#include <string>
#include <exception>
#include <atomic>
#if defined(__linux__) || defined(__FreeBSD__)
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
#endif

/*
    An IPC Mutex implementation for synchronization of processes in thread-like manner.
    SharedMutex is intended to be used as synchro primitive in classes like SharedMemory, however can be utilized by users as it is.
*/
class SharedMutex {
public:
    SharedMutex(const std::string& name) {
        #ifdef __linux__
            mutex_name = name;
        #endif
        #ifdef __FreeBSD__
            mutex_name = "/usr/local/PIPC/data/" + name; // due to shared memory specifics on FreeBSD
        #endif

        #if defined(__linux__) || defined(__FreeBSD__) 
            shm_fd = shm_open(mutex_name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
            if (shm_fd == -1) {
                std::cerr << "Error while creating shared memory" << std::endl;
                throw std::runtime_error("Error in SharedMutex");
            }
            ftruncate(shm_fd, sizeof(std::atomic<bool>));

            shared_data = (std::atomic<bool>*)(mmap(nullptr, sizeof(std::atomic<bool>), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
            if (shared_data == nullptr) {
                std::cerr << "Error while mapping shared data" << std::endl;
                throw std::runtime_error("Error in SharedMutex");
            }

            new(shared_data) std::atomic<bool>(false);  // puts atomic bool into shm
        #endif
    }

    ~SharedMutex() {
        #if defined(__linux__) || defined(__FreeBSD__)
            munmap(shared_data, sizeof(std::atomic<bool>));
            shm_unlink(mutex_name.c_str());
        #endif
    }
    SharedMutex() = default;

    SharedMutex(SharedMutex&) = delete;

    SharedMutex& operator=(SharedMutex&) = delete;

    void lock() {
        #if defined(__linux__) || defined(__FreeBSD__)
            while (true) {
                if (!shared_data->exchange(true)) {
                    return;
                }
            }
        #endif
    }

    void unlock() {
        #if defined(__linux__) || defined(__FreeBSD__)
            shared_data->store(false);
        #endif
    }

private:
    int shm_fd;
    std::string mutex_name;
    #if defined(__linux__) || defined(__FreeBSD__)
        std::atomic<bool>* shared_data;
    #endif
};
#endif