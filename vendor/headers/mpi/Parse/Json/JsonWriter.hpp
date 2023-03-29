#pragma once

#include <Parse/Json/JsonNode.hpp>
#include <Filesystem/File.hpp>

namespace Mpi {
    class JsonWriter {
    private:
        const JsonNode& rootNode;

        static void writeNode(const JsonNode& node, std::string& dst, int level = 0);
        static void writeValue(const JsonValue& value, std::string& dst, int level = 0);
        static void writePad(std::string& dst, int level);
    public:
        JsonWriter(const JsonNode& rootNode);

        void write(std::string& dst);
        void write(const File& dstFile);
    };
}