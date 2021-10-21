#include "ByteSerialize.hpp"
#include <string>
#include "string.h"

using namespace VSGE;

ByteSerialize::ByteSerialize() :
	mOutputBytes(nullptr),
	mAllocatedSize(0),
	mSerializedSize(0)
{
	Reallocate(1000);
}

ByteSerialize::~ByteSerialize() {
	SAFE_RELEASE_ARR(mOutputBytes);
}

void ByteSerialize::Reallocate(uint32 newSize) {
	byte* new_alloc = new byte[newSize];
	if (mOutputBytes != nullptr) {
		memcpy(new_alloc, mOutputBytes, mAllocatedSize);
		delete[] mOutputBytes;
	}
	mOutputBytes = new_alloc;
	mAllocatedSize = newSize;
}

void ByteSerialize::WriteBytes(void* data, uint32 size) {
	if (mAllocatedSize < mSerializedSize + size) {
		Reallocate(mAllocatedSize * 2);
	}
	memcpy(mOutputBytes + mSerializedSize, data, size);
	mSerializedSize += size;
}

void ByteSerialize::Serialize(const std::string& str) {
	for (uint32 s = 0; s < str.size(); s++) {
		Serialize(str[s]);
	}
	char zero = '\0';
	WriteBytes(&zero, 1);
}

void ByteSerialize::PopBack() {
	mSerializedSize--;
}

byte& ByteSerialize::at(uint32 index) {
	return mOutputBytes[index];
}

byte* ByteSerialize::GetBytes() {
	return mOutputBytes;
}

uint32 ByteSerialize::GetSerializedSize() {
	return mSerializedSize;
}
