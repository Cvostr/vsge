#pragma once

#include <string>
#include <vector>

namespace Mpi {

#ifdef _WIN32
typedef void* PROCESS_HANDLE;
#endif

#ifdef __linux__
typedef int PROCESS_HANDLE;
#endif

    struct ProcessCreateInfo {
        std::string executable;
        std::vector<std::string> args;
        std::string wd;
    };

    class Process {
    private:

        PROCESS_HANDLE mHandle;
    public:

        Process(PROCESS_HANDLE handle);

        int getPid();

        static Process getCurrentProcess();

        static int getCurrentProcessId();

        static Process createProcess(const ProcessCreateInfo& processCreateInfo);

        static void exit(int code);

        int waitFor();
    };

}