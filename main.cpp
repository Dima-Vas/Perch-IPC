// Tests and lib using example
#include "process_creation.h"
#include <iostream>

int main() {
    ProcessCreation myProcessCreation;
    Process myProcess("your_command", {"arg1", "arg2"});
    // Example usage:
    myProcessCreation.spawn();
    // Wait for the process to complete.
    int exitCode = myProcessCreation.wait_for_exit();

    std::cout << "Child process exited with code: " << exitCode << std::endl;

    return 0;
}
