/*
    Pipe used for output redirection.
*/

#include <PIPC/api/Process.h>
#include <PIPC/api/ProcessCreation.h>

#include <iostream>

int main(int argc, char* argv[]) {
    Process proc1("ls");
    #if defined(__linux__) || defined(__FreeBSD__)
        Process proc2("wc");
        proc2.addArgument("-l");
    #endif
    #if defined(_WIN32)
        Process proc2("Measure-Object");
        proc2.addArgument("-line");
    #endif
    ProcessCreation pc;

    Pipe pipe = pc.pipe_IPC(proc1, proc2);
    pipe.transfer(); 
    pc.wait_for_exit(proc1);
    return 0;
}
