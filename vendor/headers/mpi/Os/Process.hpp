#pragma once

#include <string>
#include <vector>
#include "PlatformHandles.hpp"
#include <IO/InputStream.hpp>
#include <IO/OutputStream.hpp>

#define PROCESS_COMBINE_STDERR 0b1
#define PROCESS_REDIRECT_STREAMS 0b10

namespace Mpi {

    struct ProcessCreateInfo {
        std::string executable;
        std::vector<std::string> args;
        std::string wd;
        int flags;

        ProcessCreateInfo() {
            flags = PROCESS_REDIRECT_STREAMS;
        }
    };

    class Process {
    private:
        InputStream*    mStdout;
        OutputStream*   mStdin;
        InputStream*    mStderr;

        PROCESS_HANDLE  mHandle;

        void destroyStreams();
    public:

        Process(PROCESS_HANDLE handle);
        Process(const Process& other) = delete;
        Process(Process&& other) noexcept;

        /// @brief Получить системный ID процесса (PID)
        /// @return PID процесса
        int getPid();

        /// @brief Получить объект, описывающий перехваченный поток стандартного вывода для дочернего процесса
        /// @return указатель на InputStream дочернего вывода
        InputStream* getStdout();

        /// @brief Получить объект, описывающий перехваченный поток стандартного ввода для дочернего процесса
        /// @return указатель на OutputStream дочернего ввода
        OutputStream* getStdin();

        /// @brief Получить объект, описывающий перехваченный поток стандартного вывода сообщений ошибок для дочернего процесса
        /// @return указатель на InputStream дочернего ввода
        InputStream* getStderr();

        static Process getCurrentProcess();

        static int getCurrentProcessId();

        static Process createProcess(const ProcessCreateInfo& processCreateInfo);

        /// @brief Завершить текущий процесс с кодом
        /// @param code код завершения
        static void exit(int code);

        /// @brief Ожидает завершения дочернего процесса
        /// @return Код возврата завершенного процесса
        /// @exception Exception в случае ошибки
        int waitFor();
    };

}