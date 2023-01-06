#pragma once

#include <stdint.h>

#define MAX_INT32 0xFFFFFFF
#define MAX_INT64 0xFFFFFFFFFFFFFFFF
#define MAX_FLOAT (3.402823466e+38f)

typedef char int8;
typedef unsigned char uint8;

typedef signed short int16;
typedef unsigned short uint16;

typedef signed int int32;
typedef unsigned int uint32;

typedef long long int64;
typedef unsigned long long uint64;

typedef int16 MpiChar;

#define null 0

#define SAFE_RELEASE(x) if (x){ delete x; x = null; }
#define SAFE_RELEASE_ARR(x) if (x) {delete[] x; x = null;}