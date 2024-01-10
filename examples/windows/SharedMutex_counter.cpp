/*
    SharedMutex used for IPC synchronization like mutex for threads.
*/

#include <PIPC/api/SharedMutex.h>
#include <PIPC/api/SharedMemory.h>

#include <iostream>
#include <windows.h>

int main() {
    SharedMutex sh_mut("MySharedMutex");
    SharedMemory<int> sh_mem{"MySharedMemory", 1};

    while (1) {
        sh_mut.lock();
        int c_read = sh_mem.read(0);
        if (c_read >= 20) {
            sh_mut.unlock();
            break;
        }
        ++c_read;
        sh_mem.write(c_read, 0);
        std::cout << "Child process: After Increment Counter = " << c_read << std::endl;
        sh_mut.unlock();
        Sleep(500);
    }

    return 0;
}
