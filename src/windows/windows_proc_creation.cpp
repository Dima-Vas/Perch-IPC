#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string.h>
#include "Process.h"
#include <semaphore.h>
#include <windows.h>
#include <string>


/*
    An auxillary POSIX-compatible function for launching a process represented by Process object.
    Acts as an end-point for launching all subprocesses.
*/

std::wstring ConvertToWideString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstrTo[0], size_needed);
    return wstrTo;
}

// Function to convert std::vector<std::string> to LPWSTR* for Windows
LPWSTR* ConvertToLPWSTRArray(const std::vector<std::string>& args) {
    LPWSTR* argv = new LPWSTR[args.size() + 1];
    int argc = args.size();
    for (int i = 0; i < argc; ++i) {
        std::wstring wideArg = ConvertToWideString(args[i]);
        argv[i] = new wchar_t[wideArg.size() + 1];
        std::copy(wideArg.begin(), wideArg.end(), argv[i]);
        argv[i][wideArg.size()] = L'\0';
    }
    argv[argc] = nullptr;
    return argv;
}

void launchChild(Process& process) {
    std::wstring executablePath = process.getPathW(); // Use wstring for Windows paths
    std::vector<std::wstring> arguments = process.getArgumentsW();

    std::wstring args;
    for (const auto& arg : arguments) {
        args += arg + L" ";
    }

    STARTUPINFO si = {};
    PROCESS_INFORMATION pi = {};

    // CreateProcessW for launching a process
    if (!CreateProcessW(
            executablePath.c_str(),
            args.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi
    )) {
        std::cerr << "Error: CreateProcess failed. Error code: " << GetLastError() << std::endl;
    }
}

/*
    POSIX-compatible function for spawning a new process.
    Returns new process id or -1 in case of error.
    Wrapped into ProcessCreation::launch
*/
int linuxLaunch(Process& process) {

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Error: Fork failed." << std::endl;
        return -1;
    }

    if (pid == 0) { // Child
        launchChild(process);
        exit(EXIT_FAILURE);
    } else { // Parent
        int status;
        process.setPID(pid);
        // std::cout << "in linuxLaunch : " << process.getPID() << std::endl;
        return pid;
    }
}

/*
    POSIX-compatible function for killing a process created from Process object with SIGTERM by default.
    Returns 0 upon success and -1 if fails.
    Wrapped into ProcessCreation::kill
*/
int linuxKill(Process& process, bool killSig = SIGTERM) {

    pid_t pid = process.getPID();

    std::cout << pid << std::endl;
    if (kill(pid, killSig) != 0) {
        std::cerr << ("Error from linuxKill : ") << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "Killed : " << pid << std::endl;
    return 0;
}


/*
    POSIX-compatible function for waiting for passed Process to terminate.
    Returns pid upon success, 0 if process has not changed its state yet and -1 if fails.
    Wrapped into ProcessCreation::wait_for_exit
*/
int linuxWaitForExit(Process& process) {
    int status = 0;
    waitpid(process.getPID(), &status, 0); // TODO : test this
    return status;
}

/*
    POSIX-compatible function for redirecting the output from Process to Process by creating a named pipe.
    Spawns processes represented by Process objects and puts them on pause until launched by Pipe::transfer.
    Returns a pointer to an array of pipe file descriptors in format {out_fd, in_fd} upon success, nullptr in case of an error.
    Wrapped into ProcessCreation::pipe_IPC
    @param out_process Process
    @param in_process Process
*/
int* linuxPipeRedirectOutput(Process& out_process, Process& in_process, const char* sem_name) {
    // TODO : Solve the bs with this one not redirecting

    pid_t output_pid;
    pid_t input_pid;
    int stdin_fd = STDIN_FILENO;
    int stdout_fd = STDOUT_FILENO;
    int * pipe_fd = new int[2];

    if (pipe(pipe_fd) == -1) {
        std::cerr << "Error from linuxPipeRedirectOutput : " << strerror(errno) << std::endl;
        return nullptr;
    }

    output_pid = fork();

    if (output_pid < 0) {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        std::cerr << "Error from linuxPipeRedirectOutput while forking : " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (output_pid == 0) { // provider
        // std::cout << "Semaphore reached with provider" << std::endl;
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            std::cerr << "Error from linuxPipeRedirectOutput : " << strerror(errno) << std::endl;
            return nullptr;
        }
        sem_t * sem = sem_open(sem_name, 0);
        if (sem == SEM_FAILED) {
            perror("Bad semaphore");
            throw std::runtime_error("Bad semaphore at provider");
        }
        close(pipe_fd[1]);
        fflush(stdout);
        sem_wait(sem);
        // std::cout << "Semaphore decremented with provider" << std::endl;
        out_process.setPID(getpid());
        launchChild(out_process);
        exit(EXIT_SUCCESS);
    }

    input_pid = fork();

    if (input_pid < 0) {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        std::cerr << "Error from linuxPipeRedirectOutput while forking : " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (input_pid == 0) { // consumer
        if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            std::cerr << "Error from linuxPipeRedirectOutput : " << strerror(errno) << std::endl;
            return nullptr;
        }
        close(pipe_fd[0]);  // Close the unused read end of the pipe

        // if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
        //     std::cerr << "Error from linuxPipeRedirectOutput : " << strerror(errno) << std::endl;
        //     return nullptr;
        // }
        close(pipe_fd[1]);
        sem_t * sem = sem_open(sem_name, 0);
        if (sem == SEM_FAILED) {
            perror("Bad semaphore");
            std::cout << sem_name << std::endl;
            throw std::runtime_error("Bad semaphore at consumer");
        }
        close(pipe_fd[0]);
        // std::cout << "Semaphore reached with consumer" << std::endl;
        fflush(stdout);
        sem_wait(sem);
        // std::cout << "Semaphore decremented with consumer" << std::endl;
        in_process.setPID(getpid());
        launchChild(in_process);
        exit(EXIT_SUCCESS);
    }

    out_process.setPID(output_pid);
    in_process.setPID(input_pid);
    return pipe_fd;
}
