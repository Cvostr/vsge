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

bool ByteSolver::end() const
{
	return mOffset >= mSize;
}

void ByteSolver::move(uint32 offset)
{
	mOffset += offset;
}

Guid ByteSolver::GetGuid()
{
	Guid id;
	Copy(&id, sizeof(Guid));
	return id;
}