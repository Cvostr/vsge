#pragma once

typedef unsigned char byte;
typedef char sbyte;

typedef byte uint8;
typedef sbyte int8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef long long int64;
typedef unsigned long long uint64;

typedef char* BaseString;

typedef void* GraphicsApiDependent;
typedef void* Buffer;

#define API_FIELD(...)

#define MAX_INT32 0xFFFFFFF
#define MAX_INT64 0xFFFFFFFFFFFFFFFF
#define MAX_FLOAT (3.402823466e+38f)

#define SAFE_RELEASE(x) if (x) delete x; x = nullptr;

#define SAFE_RELEASE_ARR(x) if (x) delete[] x; x = nullptr;