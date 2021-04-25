#include "ByteSolver.hpp"

using namespace VSGE;

void ByteSolver::Copy(void* dest, uint32 size) {
	std::memcpy(dest, mBytes + mOffset, size);
	mOffset += size;
}

std::string ByteSolver::ReadNextString() {
	std::string result;
	byte* bt = mBytes + mOffset;
	while (*bt != '\0') {
		result.push_back(*bt);
		bt++;
	}
	return result;
}