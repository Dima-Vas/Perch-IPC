/*
    Pipe used for output redirection.
*/

#include <PIPC/api/Process.h>
#include <PIPC/api/ProcessCreation.h>

#include <iostream>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    Process proc1("ls");
    Process proc2("wc");
    ProcessCreation pc;

    Pipe pipe = pc.pipe_IPC(proc1, proc2);
    pipe.transfer();
    pc.wait_for_exit(proc1); // wait only for the proc1
    return 0;
}
