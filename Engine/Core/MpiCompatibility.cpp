// Linux compatibility definitions for the prebuilt Mpi library, whose advertised
// headers are newer than the shipped libmpi.a (missing const-qualified members and
// helper functions). This file is intentionally Linux-only.
#if defined(__linux__)

#include <mpi/Core/Guid.hpp>
#include <mpi/Core/Memory.hpp>
#include <mpi/Core/Random.hpp>
#include <mpi/Os/Mutex.hpp>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <stdexcept>
#include <string>

namespace {
    uint32_t ParseHexWord(const std::string& hex, size_t offset) {
        uint32_t value = 0;
        for (size_t i = 0; i < 8; i++) {
            char ch = hex[offset + i];
            uint32_t nibble = 0;
            if (ch >= '0' && ch <= '9')
                nibble = (uint32_t)(ch - '0');
            else if (ch >= 'a' && ch <= 'f')
                nibble = (uint32_t)(ch - 'a') + 10;
            else if (ch >= 'A' && ch <= 'F')
                nibble = (uint32_t)(ch - 'A') + 10;
            else
                throw std::invalid_argument("invalid GUID hexadecimal digit");
            value = (value << 4) | nibble;
        }
        return value;
    }
}

Guid::Guid(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    Values[0] = a;
    Values[1] = b;
    Values[2] = c;
    Values[3] = d;
}

Guid::Guid(uint8_t values[16]) {
    std::memcpy(RawValues, values, sizeof(RawValues));
}

Guid::Guid(const std::string& str) {
    std::string hex;
    hex.reserve(str.size());
    for (char ch : str) {
        if (std::isxdigit((unsigned char)ch))
            hex.push_back(ch);
        else if (ch == '-' || ch == '{' || ch == '}' || ch == '(' || ch == ')' || std::isspace((unsigned char)ch))
            continue;
        else
            throw std::invalid_argument("invalid GUID character");
    }

    if (hex.size() != 32)
        throw std::invalid_argument("GUID must contain 32 hexadecimal digits");

    Values[0] = ParseHexWord(hex, 0);
    Values[1] = ParseHexWord(hex, 8);
    Values[2] = ParseHexWord(hex, 16);
    Values[3] = ParseHexWord(hex, 24);
}

bool Guid::operator==(const Guid& other) const {
    return Values[0] == other.Values[0]
        && Values[1] == other.Values[1]
        && Values[2] == other.Values[2]
        && Values[3] == other.Values[3];
}

bool Guid::operator!=(const Guid& other) const {
    return !(*this == other);
}

bool Guid::isValid() const {
    return Values[0] != 0 || Values[1] != 0 || Values[2] != 0 || Values[3] != 0;
}

void Memcpy(void* dst, const void* src, size_t size) {
    std::memcpy(dst, src, size);
}

namespace Mpi {

void Mutex::lock() const {
    pthread_mutex_lock(static_cast<pthread_mutex_t*>(mHandle));
}

void Mutex::unlock() const {
    pthread_mutex_unlock(static_cast<pthread_mutex_t*>(mHandle));
}

int Random::random() {
    return std::rand();
}

int Random::random(int min, int max) {
    if (max < min)
        std::swap(min, max);
    return min + (std::rand() % (max - min + 1));
}

double Random::randomNormalized() {
    return (double)std::rand() / (double)RAND_MAX;
}

std::string Random::randomString(uint32_t len) {
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string result;
    result.reserve(len);
    for (uint32_t i = 0; i < len; i++)
        result.push_back(alphabet[std::rand() % (sizeof(alphabet) - 1)]);
    return result;
}

}

#endif
