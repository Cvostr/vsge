#pragma once

#include <Core/Disposable.hpp>
#include <string>

namespace Mpi {
    class OutputStream : public Disposable {
    public:

        OutputStream() {

        }

        virtual ~OutputStream() {

        }

        /// @brief Записать в поток массив данных с указанным количеством байт 
        /// @param buffer массив данных
        /// @param len размер данных для записи в байтах
        /// @return реальное количество записанных байт
        virtual int write(const char* buffer, int len) = 0;

        /// @brief Записать в поток строку
        /// @param str строка для записи
        /// @return количество записанных байт
        int write(const std::string& str);

        /// @brief Записать в поток строку и символ переноса строки
        /// @param str строка для записи
        /// @return количество записанных байт
        int writeln(const std::string& str);
    };
}