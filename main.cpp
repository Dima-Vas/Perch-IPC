// Tests and lib using example
#include "process_creation.h"
#include <iostream>

int main() {
    ProcessCreation myProcess;

    // Example usage:
    myProcess.spawn("your_command", {"arg1", "arg2"});
    // Wait for the process to complete.
    int exitCode = myProcess.wait_for_exit();

    std::cout << "Child process exited with code: " << exitCode << std::endl;

    return 0;
}
