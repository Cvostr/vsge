#pragma once

#include "JsonNode.hpp"
#include <Parse/CharBuffer.hpp>

namespace Mpi {

    class JsonReader {
    private:
        CharBuffer* charBuffer;
        JsonNode* readSingleValueNode();
        JsonValue* readBody();
        JsonValue* readArray();
        JsonValue* readValue();
        JsonValue* readNumber();

        std::string readLabel();
        std::string readIdentifier();
        std::string readUntil(char c);
    public:

        JsonReader(const std::string& jsonData);

        ~JsonReader();

        JsonNode& parse();
    };

}