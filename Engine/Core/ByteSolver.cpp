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

uint64 ByteSolver::offset() const
{
	return mOffset;
}

void ByteSolver::move(uint32 offset)
{
	mOffset += offset;
}

byte* ByteSolver::current()
{
	return mBytes + mOffset;
}

Guid ByteSolver::GetGuid()
{
	Guid id;
	Copy(&id, sizeof(Guid));
	return id;
}