#include "Shell.hpp"
#include <iostream>
#include <array>
#include <cstdio>
#include <Core/Logger.hpp>

using namespace VSGE;

std::string VSGE::ExecuteShellCommand(const std::string& command) {
    std::array<char, 1024> buffer;
    const char* cmd = command.c_str();
    std::string result;
    const std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe)
    {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Can't open shell file\n";
        return result;
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    return result;
}