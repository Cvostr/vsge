#include "ByteSolver.hpp"
#include "string.h"

using namespace VSGE;

void ByteSolver::Copy(void* dest, uint32 size) {
	memcpy(dest, mBytes + mOffset, size);
	mOffset += size;
}

std::string ByteSolver::ReadNextString() {
	std::string result;
	byte* bt = mBytes + mOffset;
	while (*bt != '\0') {
		result.push_back(*bt);
		bt++;
		mOffset++;
	}
	mOffset++;

	return result;
}
