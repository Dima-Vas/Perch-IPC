/**
 * @file Pipe.h
 * @brief This file contains the declaration of Pipe class.
 */

#ifndef MY_BOOST_PROCESS_PIPE_H
#define MY_BOOST_PROCESS_PIPE_H

#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <random>
#include <cstring>

#include "Process.h"

#if defined(__linux__) || defined(__FreeBSD__)
    #include <semaphore.h>
    #include <unistd.h>
    #include "../linux/linux_proc_creation.h"
    #include <fcntl.h>
    #include <sys/mman.h>
#endif
#if defined(_WIN32)
    #include <io.h>
    #include <windows.h>
    #include "../windows/windows_proc_creation.h"
#endif


/**
 * @class Pipe
 * @brief A container class representing an entity of a pipe.
 *
 * Provides the interface for the organization of IPC using named pipes.
 * Represents a created Pipe and stores information about it.
 * @warning This class is not intended to be initialized directly. Check ProcessCreation API for 
 * organizing IPC via pipes.
 */
class Pipe {
public:

    /**
     * @brief Default constructor for Pipe class.
     */
    Pipe() {
        init_named_semaphore();
    }

    /**
     * @brief Basic constructor for Process class.
     * @param pipe_fd a pair of descriptors in {input_fd, output_fd} format in form of an array.
     */
    Pipe(int pipe_fd[2]) {
        init_named_semaphore();
        input_fd = pipe_fd[0];
        output_fd = pipe_fd[1];
    }

    ~Pipe() {
        #if defined(__linux__) || defined(__FreeBSD__)
            sem_unlink(launch_sem_name.c_str());
            sem_destroy(launch_sem);
        #endif
        #if defined(_WIN32)
            ReleaseSemaphore(launch_sem, 1, NULL);
        #endif
    }

    int get_input_fd() {
        return input_fd;
    }

    int get_output_fd() {
        return output_fd;
    }

    Process get_input_Process() {
        return input_process;
    }

    Process get_output_Process() {
        return output_process;
    }

    /**
     * @brief Connects two Process objects via named pipe.
     * @param out_proc The process to be an outputting one.
     * @param in_proc The process to be an inputting one.
     */
    int connect(Process& out_proc, Process& in_proc) {
        #if defined(__linux__) || defined(__FreeBSD__)
            original_stdin = dup(STDIN_FILENO);
            original_stdout = dup(STDOUT_FILENO);
            int* created_pipe = linuxPipeRedirectOutput(out_proc, in_proc, launch_sem_name.c_str());
            output_fd = created_pipe[1];
            input_fd = created_pipe[0];
            output_process = out_proc;
            input_process = in_proc;
        #endif
        #if defined(_WIN32)
            original_stdin = _dup(_fileno(stdin));
            original_stdout = _dup(_fileno(stdout));
            int* created_pipe = windowsPipeRedirectOutput(out_proc, in_proc, launch_sem_name.c_str());
            output_fd = created_pipe[1];
            input_fd = created_pipe[0];
            output_process = out_proc;
            input_process = in_proc;
        #endif
        return 0;
    }

    /**
     * @brief Launches two Processes linked by this Pipe. The output of Pipe::input_process is transfered to the Pipe::output_process.
     */
    int transfer() {
        #if defined(__linux__) || defined(__FreeBSD__)
            if (sem_post(launch_sem) != 0) {
                perror("Error when post from Pipe");
                return -1;
            }
            if (sem_post(launch_sem) != 0 ) {
                perror("Error when post from Pipe");
                return -1;
            }
            if (dup2(original_stdin, STDIN_FILENO) == -1) {
                perror("Error restoring stdin");
                return -1;
            }

            if (dup2(original_stdout, STDOUT_FILENO) == -1) {
                perror("Error restoring stdout");
                return -1;
            }

        close(original_stdin);
        close(original_stdout);
        #endif
        #if defined(_WIN32)
            if (!ReleaseSemaphore(
                    launch_sem,
                    1,
                    NULL)) {
                std::cerr << "Error when releasing semaphore: " << GetLastError() << std::endl;
                return -1;
            }
        #endif
        return 0;
    }

private:
    int output_fd;
    int input_fd;
    int original_stdout;
    int original_stdin;
    Process output_process;
    Process input_process;
    #if defined(__linux__) || defined(__FreeBSD__)
        sem_t * launch_sem;
    #endif
    #if defined(_WIN32)
        HANDLE launch_sem;
    #endif
    std::string launch_sem_name;

    int init_named_semaphore() {
        generate_unique_name();
        
        #if defined(__linux__) || defined(__FreeBSD__)
            launch_sem = sem_open(launch_sem_name.c_str(), O_CREAT | O_EXCL , 0644, 0);
            if (launch_sem == SEM_FAILED) {
                perror("Bad semaphore");
                throw std::runtime_error("Bad semaphore at Pipe");
            }
        #endif
        #if defined(_WIN32)
            launch_sem = CreateSemaphore(
                    NULL,
                    0,
                    1,
                    launch_sem_name.c_str()
            );
            if (launch_sem == NULL) {
                std::cerr << "Error creating semaphore: " << GetLastError() << std::endl;
                return -1;
            }
        #endif
        return 0;
    }

    void generate_unique_name() {
        const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(0, chars.size() - 1);
        std::ostringstream oss;
        #if defined(__linux__) || defined(__FreeBSD__)
            oss << "/" << getpid() << "_";
        #endif
        #if defined(_WIN32)
            oss << "/" << GetCurrentProcessId() << "_";
        #endif
        std::string random_string = oss.str();

        for (std::size_t i = 0; i < chars.length(); ++i) {
            random_string += chars[distribution(generator)];
        }

        launch_sem_name = random_string;
    }
};

#endif //MY_BOOST_PROCESS_PIPE_H
