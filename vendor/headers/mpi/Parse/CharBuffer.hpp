#pragma once

#include <Core/Base.hpp>
#include <string>

namespace Mpi {
    class CharBuffer {
    private:
        char* buffer;
        mpi_size_t size;
        mpi_pos_t pos;
    public:
        CharBuffer(const char* buffer, mpi_size_t size); 
        CharBuffer(const std::string& stringBuffer); 
        ~CharBuffer();

        CharBuffer(const CharBuffer& charBuffer);
        CharBuffer(CharBuffer&& charBuffer);

        char nextChar();
        char getChar() const;

        mpi_size_t getSize() const;
        mpi_pos_t getPos() const;

        void unchar();
        void reset();
        int skipSpaces();
        bool isEnd() const;

        std::string readUntil(char _char);
        std::string readUntil(char* chars, int count);
    };
}