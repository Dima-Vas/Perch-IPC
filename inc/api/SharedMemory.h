/**
 * @file SharedMemory.h
 * @brief This file contains the declaration of SharedMemory class.
 */

#ifndef MY_BOOST_PROCESS_SHARED_MEMORY_H
#define MY_BOOST_PROCESS_SHARED_MEMORY_H

#ifdef __FreeBSD__
    #define __BEGIN_DECLS extern "C" { // somehow stdc++ messes with defines on FreeBSD
    #define __END_DECLS }
    #include <sys/stat.h>
#endif
#if defined(_WIN32)
    #include <windows.h>
#endif

#include "SharedMutex.h"

#include <string>
#include <semaphore>
#include <cstring>
#include <memory>

/**
 * @class SharedMemory
 * @brief A class for management and representation of shared memory chunk.
 *
 * Initializes shared memory chunk of static size, provides an API allowing to manipulate it excluding the
 * possibility of any race conditions and unmaps the resources after no longer needed.
 * @warning This class does not support default constructors.
 */
template <typename T>
class SharedMemory {
public:

    /**
     * @brief Basic constructor for SharedMemory class.
     * @param aName unique string name for shared memory identification.
     * @param aSize maximal number of elements that this chunk can contain.
     */
    SharedMemory(const std::string& aName, size_t aSize) :
        size(aSize),
        capacity(0),
        frozen(false),
        sh_mutex(aName + "Mutex"),
        data(nullptr)
    {
        #ifdef __FreeBSD__
            name = "/usr/local/PIPC/data/" + name; // due to shared memory specifics on FreeBSD
        #endif
        #if defined(__linux__) || defined(__FreeBSD__)
            const std::string mutex_name = aName + "_mutex";
            mem_fd = shm_open(name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
            if (mem_fd < 0) {
                std::cerr << "Cannot get shared memory" << strerror(errno) <<  std::endl;
                throw std::runtime_error("Error in SharedMemory");
            }
            data = (T*)(mmap(nullptr, size * sizeof(T), PROT_READ | PROT_WRITE,  MAP_SHARED, mem_fd, 0));
            if (data == MAP_FAILED) {
                close(mem_fd);
                std::cerr << "Cannot get memory map : " << strerror(errno) << std::endl;
                throw std::runtime_error("Error in SharedMemory");
            }
            ftruncate(mem_fd, size * sizeof(T));
        #endif
        #if defined(_WIN32)
            std::wstring wname(aName.begin(), aName.end());
            std::wstring prefix = L"Global\\";
            std::wstring combinedName = prefix + wname;
            name = combinedName;
            mem_fd = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
            if (mem_fd == nullptr) {
                DWORD lastError = GetLastError();
                if (lastError == ERROR_FILE_NOT_FOUND) {
                    mem_fd = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size * sizeof(T), name.c_str());
                    if (mem_fd == nullptr) {
                        std::cerr << "Cannot get shared memory at all: " << GetLastError() << std::endl;
                        throw std::runtime_error("Error in SharedMemory");
                    }
                } else {
                    std::cerr << "Cannot get shared memory: " << lastError << std::endl;
                    throw std::runtime_error("Error in SharedMemory");
                }
            }
            data = static_cast<T*>(MapViewOfFile(mem_fd, FILE_MAP_ALL_ACCESS, 0, 0, size * sizeof(T)));
            if (data == nullptr) {
                CloseHandle(mem_fd);
                std::cerr << "Cannot map data of SharedMemory: " << GetLastError() << std::endl;
                throw std::runtime_error("Error in SharedMemory");
            }
        #endif
    };

    ~SharedMemory() {
        #ifdef  __linux__
            if (shm_unlink(name.c_str()) < 0) {
                std::cerr << "Could not unlink the memory in SharedMemory" << std::endl;
            };
            if (munmap(data, size * sizeof(T)) < 0) {
                std::cerr << "Could not unmap the data in SharedMemory" << std::endl;
            };
            if (close(mem_fd) < 0) {
                std::cerr << "Could not close the memory descriptor in SharedMemory" << std::endl;
            }
        #endif
        #if defined(_WIN32)
            UnmapViewOfFile(data);
            CloseHandle(mem_fd);
        #endif
    };

    SharedMemory(SharedMemory&) = delete;

    /**
     * @brief Writes new element to the chunk on the given index.
     * @param to_write an element to write
     * @param idx an index of the element to write. If -1, writes to the first free position in chunk.
     */
    void write(const T& to_write, size_t idx) {
        sh_mutex.lock();
        if (frozen) {
            sh_mutex.unlock();
            std::cerr << "Write to frozen SharedMemory" << std::endl;
            throw std::runtime_error("Error in SharedMemory");
        }
        if (idx >= size) {
            sh_mutex.unlock();
            std::cerr << "Bad index while writing to SharedMemory" << std::endl;
            throw std::runtime_error("Error in SharedMemory");
        }
        if (idx == -1) {
            if (capacity >= size) {
                sh_mutex.unlock();
                std::cerr << "Cannot write to SharedMemory, capacity exceeded" << std::endl;
                throw std::runtime_error("Error in SharedMemory");
            }
            data[capacity++] = to_write;
        } else if (idx < size) {
            data[idx] = to_write;
        } else {
            sh_mutex.unlock();
            std::cerr << "Bad index while writing to SharedMemory" << std::endl;
            throw std::runtime_error("Error in SharedMemory");
        }
        sh_mutex.unlock();
    }

    /**
     * @brief Reads an element on the given index.
     * @param idx an index of the element to read.
     * @return The current value on ith index.
     */
    T& read(size_t idx) {
        sh_mutex.lock();
        if (idx >= size) {
            std::cerr << "Bad index while reading from SharedMemory" << std::endl;
            throw std::runtime_error("Error in SharedMemory");
        }
        T& to_ret = data[idx];
        sh_mutex.unlock();
        return to_ret;
    }

    /**
     * @brief Returns the previous value of the idx'th element of data, swapping it with new_value.
     * Should be used as CAS operation, i.e. combined read-write operation.
     * @param idx an index of the element to compare-and-swap.
     * @param new_value the new value to swap with existing one.
     * @return The value swapped out from the memory.
     */
    T& compare_and_swap(size_t idx, T& new_value) {
        sh_mutex.lock();
        if (frozen) {
            sh_mutex.unlock();
            std::cerr << "CAS to frozen SharedMemory" << std::endl;
            throw std::runtime_error("Error in SharedMemory");
        }
        if (idx >= size) {
            sh_mutex.unlock();
            std::cerr << "Bad index while using compare_and_swap in SharedMemory" << std::endl;
            throw std::runtime_error("Error in SharedMemory");
        }

        T& current_value = data[idx];
        data[idx] = new_value;
        sh_mutex.unlock();

        return current_value;
    }

    /**
     * @brief Makes the memory unable to be written. When trying to write or compare-and-swap to 
     * a "frozen" memory, runtime error is raised.
     */
    void freeze() {
        sh_mutex.lock();
        frozen = true;
        sh_mutex.unlock();
    }

    bool is_frozen() {
        return frozen;
    }

    size_t get_size() {
        return size;
    }

private :
    
    size_t size;
    size_t capacity;
    bool frozen;
    SharedMutex sh_mutex;
    T* data;
    #ifdef _WIN32
       HANDLE mem_fd;
       std::wstring name;
    #endif
    #ifdef __linux__
        int mem_fd;
        std::string name;
    #endif
};

#endif //MY_BOOST_PROCESS_SHARED_MEMORY_H
