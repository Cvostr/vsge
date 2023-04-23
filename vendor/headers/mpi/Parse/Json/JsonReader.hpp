#pragma once

#include "JsonNode.hpp"
#include <Parse/CharBuffer.hpp>
#include "JsonLexer.hpp"

namespace Mpi {

    class JsonReader {
    private:
        JsonLexer* lexer;
        JsonNode* readSingleValueNode();
        JsonValue* readBody();
        JsonValue* readArray();
        JsonValue* readValue();

        std::string readIdentifier();
        std::string readUntil(char c);
    public:

        JsonReader(const std::string& jsonData);

        ~JsonReader();

        JsonNode& parse();
    };

}