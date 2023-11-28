
#ifndef SHARED_MUTEX_H
#define SHARED_MUTEX_H


#if _WIN32
#endif

#ifdef __linux__

#include <sys/mman.h>
#include <sys/mman.h>

#endif


#include <mutex>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <exception>
#include <atomic>

/*
    An IPC Mutex implementation for synchronization of processes in thread-like manner.
    SharedMutex is intended to be used as synchro primitive in classes like SharedMemory, however can be utilized by users as it is.
*/
class SharedMutex {
public:
    SharedMutex(const std::string& name) {
        mutex_name = name;

        #ifdef __linux__
            shm_fd = shm_open(mutex_name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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
        #ifdef __linux__
            munmap(shared_data, sizeof(std::atomic<bool>));
            shm_unlink(mutex_name.c_str());
        #endif
    }

    void lock() {
        #ifdef __linux__
            while (true) {
                if (!shared_data->exchange(true)) {
                    return;
                }
            }
        #endif
    }

    void unlock() {
        #ifdef __linux__
            shared_data->store(false);
        #endif
    }

private:
    int shm_fd; 
    int mutex_and_flag_shm_fd;
    std::string mutex_name;
    #ifdef __linux__
        std::atomic<bool>* shared_data;
    #endif
};
#endif