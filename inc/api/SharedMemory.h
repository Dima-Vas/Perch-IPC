
#ifndef MY_BOOST_PROCESS_SHARED_MEMORY_H
#define MY_BOOST_PROCESS_SHARED_MEMORY_H

#ifdef __FreeBSD__
    #define __BEGIN_DECLS extern "C" {
    #define __END_DECLS }
#endif

#include "SharedMutex.h"

#include <string>
#include <semaphore>
#include <cstring>
#include <memory>

template <typename T>
class SharedMemory {
public:
    SharedMemory(const std::string& aName, size_t aSize) :
        name(aName),
        size(aSize),
        capacity(0),
        frozen(false),
        sh_mutex(aName + "_mutex"),
        mem_fd(-1),
        data(nullptr)
    {
#ifdef __linux__
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
    };

    SharedMemory(SharedMemory&) = delete;

    /*
     * Writes the to_write to idx while preventing the racing write from another process
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

    /*
     * Returns the previous value of the idx'th element of data, swapping it with new_value.
     * Should be used as CAS operation, i.e. combined read-write operation.
     */
    T& compare_and_swap(size_t idx, const T& new_value) {
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

    /*
     * Make memory unable to be written anymore.
     */
    void freeze() {
        frozen = true;
    };

    bool is_frozen() {
        return frozen;
    }

    size_t get_size() {
        return size;
    }

private :
    std::string name;
    size_t size;
    size_t capacity;
    bool frozen;
    SharedMutex sh_mutex;
    int mem_fd;
    T* data;
};

#endif //MY_BOOST_PROCESS_SHARED_MEMORY_H
