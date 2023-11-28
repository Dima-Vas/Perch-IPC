
#ifndef MY_BOOST_PROCESS_SHARED_MEMORY_H
#define MY_BOOST_PROCESS_SHARED_MEMORY_H


#if _WIN32
#endif

#ifdef __linux__
#endif



#include "SharedMutex.h"

template <typename T>
class SharedMemory {
public:
    SharedMemory(const std::string& aName, size_t aSize, int aAccess) {
        name = aName;
        size = aSize;
        access = aAccess;
    }
    ~SharedMemory() {};

    void write(const T& data, size_t idx = -1) {
        if (idx == -1) {
            idx = capacity;
        }
    };
private :
    std::string name;
    size_t size;
    size_t capacity;
    int access;
    SharedMutex sh_mutex;
};

#endif //MY_BOOST_PROCESS_SHARED_MEMORY_H
