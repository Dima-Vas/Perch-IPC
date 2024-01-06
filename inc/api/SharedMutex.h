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
#if defined(_WIN32)
    #include <windows.h>
    #include <synchapi.h>
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
        #if defined(_WIN32)
            std::wstring wname(name.begin(), name.end());
            std::wstring prefix = L"Global\\";
            std::wstring combinedName = prefix + wname;
            mutex_name = combinedName;
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
        #if defined(_WIN32)
            mutex_handle = CreateMutexW(nullptr, FALSE, mutex_name.c_str());
            if (mutex_handle == NULL) {
                std::cerr << "Error while creating mutex: " << GetLastError() << std::endl;
                throw std::runtime_error("Error in SharedMutex");
            }
        #endif
    }

    ~SharedMutex() {
        #if defined(__linux__) || defined(__FreeBSD__)
            munmap(shared_data, sizeof(std::atomic<bool>));
            shm_unlink(mutex_name.c_str());
        #endif
        #if defined(_WIN32)
            if (mutex_handle != NULL) {
                CloseHandle(mutex_handle);
            }
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
        #if defined(_WIN32)
            DWORD dwWaitResult = WaitForSingleObject(mutex_handle, INFINITE);
            if (dwWaitResult != WAIT_OBJECT_0) {
                std::cerr << "Error while waiting for mutex: " << GetLastError() << std::endl;
                throw std::runtime_error("Error in SharedMutex");
            }
        #endif
    }

    void unlock() {
        #if defined(__linux__) || defined(__FreeBSD__)
            shared_data->store(false);
        #endif
        #if defined(_WIN32)
            ReleaseMutex(mutex_handle);
        #endif
    }

private:
    int shm_fd;
    #if defined(__linux__) || defined(__FreeBSD__)
        std::string mutex_name;
        std::atomic<bool>* shared_data;
    #endif
    #if defined(_WIN32)
        HANDLE mutex_handle;
        std::wstring mutex_name;
    #endif
};
#endif