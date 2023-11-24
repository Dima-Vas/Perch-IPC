// Tests and lib using example
#include "process_creation.h"
#include "linux_proc_creation.h"
#include "Pipe.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Process proc1("echo");
    Process proc2("wc");
    ProcessCreation pc;
    proc1.addArgument("Hello world");
    Pipe pipe = pc.pipe_redirect_output(proc1, proc2);
    pipe.transfer(); // this simply increments the semaphore inside of Pipe and checks for errors
    return 0;
}
