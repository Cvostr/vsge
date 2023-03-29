#pragma once

#include "JsonValue.hpp"
#include <type_traits>

namespace Mpi {
    class JsonNode {
    private:
        std::string key;

        JsonValue value;

    public:
        JsonNode();
        JsonNode(const std::string& key);
        JsonNode(const JsonValue& value);
        JsonNode(const std::string& key, const JsonValue& value);
        ~JsonNode();
        
        bool isNullValue() const;

        template<typename T>
        T getValue() const;

        const JsonValue& getValue() const;

        const JsonValueType getValueType() const;

        const std::string& getKey() const;

        const size_t size() const;
        const JsonNode& operator[](const mpi_pos_t index) const;
        const JsonNode& operator[](const std::string& label) const;

        JsonNode& add(const std::string& key, const JsonValue& value);
        JsonNode& add(const JsonValue& value);
        JsonNode& add(const JsonNode& node);

        static const JsonNode& object();
        static const JsonNode& array();
    };
}