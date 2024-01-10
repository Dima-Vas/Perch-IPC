#include <iostream>
#include <vector>
#include <windows.h>
#include <io.h>
#include <string>
#include "windows_proc_creation.h"

std::wstring ConvertToWideString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstrTo[0], size_needed);
    return wstrTo;
}

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


int windowsLaunch(Process& process) {
    std::wstring executablePath = ConvertToWideString(process.getPath());
    std::vector<std::string> arguments = process.getArguments();
    LPWSTR* argv = ConvertToLPWSTRArray(arguments);
    std::wstring args;
    for (int i = 0; argv[i] != nullptr; ++i) {
        args += std::wstring(argv[i]) + L" ";
    }
    STARTUPINFOW si = {};
    PROCESS_INFORMATION pi = {};
    if (!CreateProcessW(
            NULL,
            &args[0],
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
    )) {
        std::cerr << "Error: CreateProcess failed. Error code: " << GetLastError() << std::endl;
        throw new std::runtime_error("Bad windowsLaunch");
    }
    process.setPID(pi.hProcess);
    for (int i = 0; argv[i] != nullptr; ++i) {
        delete[] argv[i];
    }
    return 0;
}


int windowsKill(Process& process, int killSig) {
    DWORD pid = GetProcessId(process.getPID());
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Error: Unable to open process. Error code: " << GetLastError() << std::endl;
        return -1;
    }
    if (!TerminateProcess(hProcess, killSig)) {
        std::cerr << "Error: Unable to terminate process. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }
    CloseHandle(hProcess);
    return 0;
}


int windowsWaitForExit(Process& process) {
    DWORD pid = GetProcessId(process.getPID());
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Error: Unable to open process. Error code: " << GetLastError() << std::endl;
        return -1;
    }
    DWORD waitResult = WaitForSingleObject(hProcess, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        std::cerr << "Error: WaitForSingleObject failed. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }
    DWORD exitCode = 0;
    GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);
    return static_cast<int>(exitCode);
}


int* windowsPipeRedirectOutput(Process& out_process, Process& in_process, const char* sem_name) {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
        std::cerr << "Error creating output pipe" << std::endl;
        return nullptr;
    }
    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        std::cerr << "Error setting handle information" << std::endl;
        return nullptr;
    }
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    BOOL bSuccess = FALSE;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    bSuccess = CreateProcessA(
            NULL,
            const_cast<LPSTR>(out_process.getPath().c_str()),
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &siStartInfo,
            &piProcInfo
    );
    if (!bSuccess) {
        std::cerr << "Error creating process: " << GetLastError() << std::endl;
        return nullptr;
    }
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    int* pipe_fd = new int[2];
    pipe_fd[0] = _open_osfhandle(reinterpret_cast<intptr_t>(g_hChildStd_OUT_Rd), 0);
    pipe_fd[1] = _open_osfhandle(reinterpret_cast<intptr_t>(g_hChildStd_OUT_Wr), 0);
    return pipe_fd;
}
