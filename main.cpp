// Tests and lib using example
#include "process_creation.h"
#include "linux_proc_creation.h"
#include "Pipe.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Process proc1("ls");
    Process proc2("echo");
    ProcessCreation pc;
    proc1.addArgument("../");
    // proc1.addArgument("HEHEHEH");
    Pipe pipe = pc.pipe_redirect_output(proc2, proc1);
    pipe.transfer(); // this simply increments the semaphore inside of Pipe and checks for errors
    return 0;
}
