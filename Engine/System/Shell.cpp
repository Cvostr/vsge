#include "Shell.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <Core/Logger.hpp>

using namespace VSGE;

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

std::string VSGE::ExecuteShellCommand(const std::string& command) {
    char buffer[1024];
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    }
    catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}
