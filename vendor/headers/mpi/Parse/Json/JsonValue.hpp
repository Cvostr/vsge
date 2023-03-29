#pragma once

#include <Core/Base.hpp>
#include <string>
#include <vector>

namespace Mpi {
    enum JsonValueType {
        JSON_VALUE_TYPE_INT,
        JSON_VALUE_TYPE_DOUBLE,
        JSON_VALUE_TYPE_BOOL,
        JSON_VALUE_TYPE_NULL,
        JSON_VALUE_TYPE_STRING,
        JSON_VALUE_TYPE_OBJECT,
        JSON_VALUE_TYPE_ARRAY
    };

    class JsonNode;
    class JsonWriter;

    typedef std::vector<JsonNode> JsonNodesList;

    class JsonValue {
        JsonValueType           type;

        union {
            int                 intValue;
            double              doubleValue;
            bool                boolValue;
        };

        std::string         stringValue;
        JsonNodesList       children;

        friend class JsonNode;
        friend class JsonWriter;

        JsonValue(JsonValueType type);

    public:
        JsonValue();
        JsonValue(int value);
        JsonValue(double value);
        JsonValue(bool value);
        JsonValue(const std::string& value);
        JsonValue(const char* value);
        JsonValue(const JsonNodesList& children);
        ~JsonValue();

        JsonValue(const JsonValue& other);
        JsonValue(JsonValue&& other) noexcept;

        const JsonValueType getType() const;

        JsonValue& add(const std::string& key, const JsonValue& value);
        JsonValue& add(const JsonValue& value);

        static JsonValue object();
        static JsonValue array();
    };
}