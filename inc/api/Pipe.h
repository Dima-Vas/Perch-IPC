
#ifndef MY_BOOST_PROCESS_PIPE_H
#define MY_BOOST_PROCESS_PIPE_H

#include <string>
#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <ctime>
#include <iomanip>
#include <random>


#include "Process.h"
#include <semaphore.h>

#ifdef __linux__
    #include "../linux/linux_proc_creation.h"
    #include <unistd.h>
#endif

/*
    Provides the interface for the organization of IPC using named pipes.
    Represents a created Pipe and stores information about it.
*/
class Pipe {
public:
    Pipe() {
        init_named_semaphore();
    }
    Pipe(int pipe_fd[2]) {
        init_named_semaphore();
        input_fd = pipe_fd[0];
        output_fd = pipe_fd[1];
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

    /*
        Connects two Process objects via monodirectional anonymous pipe.
    */
    int connect(Process& out_proc, Process& in_proc) {
        #ifdef __linux__
            original_stdin = dup(STDIN_FILENO);
            original_stdout = dup(STDOUT_FILENO);
            int* created_pipe = linuxPipeRedirectOutput(out_proc, in_proc, launch_sem_name.c_str());
            if (created_pipe == nullptr) {
                return -1;
            }
            output_fd = created_pipe[1];
            input_fd = created_pipe[0];
            output_process = out_proc;
            input_process = in_proc;
        #endif
        return 0;
    }

    /*
        Launches the Processes linked by this Pipe. The output of Pipe::input_process is transfered to the Pipe::output_process.
    */
    int transfer() {
        if (sem_post(launch_sem) != 0) {
            perror("Error when post from Pipe");
            return -1;
        }
        if (sem_post(launch_sem) != 0 != 0) {
            perror("Error when post from Pipe");
            return -1;
        }
        return 0;
    }
private:
    int output_fd;
    int input_fd;
    int original_stdout;
    int original_stdin;
    Process output_process;
    Process input_process;
    sem_t * launch_sem;
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
        return 0;
    }

    void generate_unique_name() {
        const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(0, chars.size() - 1);
        std::ostringstream oss;
        oss << "/" << getpid() << "_";
        std::string random_string = oss.str();

        for (std::size_t i = 0; i < chars.length(); ++i) {
            random_string += chars[distribution(generator)];
        }
        
        launch_sem_name = random_string;
    }
};

#endif //MY_BOOST_PROCESS_PIPE_H
