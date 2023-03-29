#pragma once

#include <Core/Base.hpp>
#include <string>

namespace Mpi {

    enum TokenType {
        TOKEN_TYPE_NUMBER,
        TOKEN_TYPE_STRING,
        TOKEN_TYPE_PUNCT
    };

    class Token {
    private:
        TokenType type;
        mpi_pos_t pos;
        std::string token_str;
        char token_char;
    public:
        Token(TokenType type, mpi_pos_t pos, const std::string& str);
        Token(TokenType type, mpi_pos_t pos, const char chr);

        ~Token() {}

        TokenType getType() const;
        mpi_pos_t getPos() const;

        const std::string& getString() const;
        const char getChar() const;
    };
}