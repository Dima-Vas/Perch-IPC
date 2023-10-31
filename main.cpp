// Tests and lib using example
#include "process_creation.h"
#include <iostream>

int main() {
    ProcessCreation myProcessCreation;
    Process myProcess("your_command");
    myProcess.addArgument("arg1");
    myProcess.addArgument("arg2");
    myProcessCreation.spawn(myProcess);
    int exitCode = myProcessCreation.wait_for_exit(myProcess);

    std::cout << "Child process exited with code: " << exitCode << std::endl;

    return 0;
}
