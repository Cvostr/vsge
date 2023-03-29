#pragma once

#include <stdint.h>
#include <stddef.h>
#include "Exception.hpp"

#define MAX_INT32 0xFFFFFFF
#define MAX_INT64 0xFFFFFFFFFFFFFFFF
#define MAX_FLOAT (3.402823466e+38f)

#define null 0

#define SAFE_RELEASE(x) if (x){ delete x; x = null; }
#define SAFE_RELEASE_ARR(x) if (x) {delete[] x; x = null;}

#define ERRC -1

typedef unsigned long long mpi_size_t;
typedef unsigned long long mpi_pos_t;

enum StreamSeekDir {
	SEEKDIR_SET = 0,
	SEEKDIR_CUR = 1,
	SEEKDIR_END = 2
};