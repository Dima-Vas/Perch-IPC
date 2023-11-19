//
// Created by Admin on 31.10.2023.
//

#ifndef MY_BOOST_PROCESS_PIPE_H
#define MY_BOOST_PROCESS_PIPE_H
#include "Process.h"
#include "linux_proc_creation.h"

/*
    Provides the interface for the organization of IPC using named pipes.
    Represents a created Pipe and stores information about it. 
*/
class Pipe {
public:
    Pipe() {}
    Pipe(int pipe_fd[2]) {
        input_fd = pipe_fd[0];
        output_fd = pipe_fd[1];
    }

    Pipe(int pipe_fd[2], Process out_proc, Process in_proc) {
        input_fd = pipe_fd[0];
        output_fd = pipe_fd[1];
        input_process = in_proc;
        output_process = out_proc;
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
        Connects two Process objects via monodirectional named pipe.
    */
    int connect(Process out_proc, Process in_proc) {
        #ifdef __linux__
        int* created_pipe = linuxPipeRedirectOutput(out_proc, in_proc);
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
private:
    int output_fd;
    int input_fd;
    Process output_process;
    Process input_process;
};

#endif //MY_BOOST_PROCESS_PIPE_H
