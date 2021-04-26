#include "ByteSerialize.hpp"
#include <string>

using namespace VSGE;

void ByteSerialize::Reallocate(uint32 newSize) {
	byte* new_alloc = new byte[newSize];
	if (mOutputBytes != nullptr) {
		memcpy(new_alloc, mOutputBytes, mAllocatedSize);
		delete[] mOutputBytes;
	}
	mOutputBytes = new_alloc;
	mAllocatedSize = newSize;
}

void ByteSerialize::Serialize(void* data, uint32 size) {
	if (mAllocatedSize < mSerializedSize + size) {
		Reallocate(mAllocatedSize * 2);
	}
	memcpy(mOutputBytes + mSerializedSize, data, size);
}

void ByteSerialize::Serialize(std::string str) {
	for (uint32 s = 0; s < str.size(); s++) {
		Serialize(str[s]);
	}
	Serialize("\0");
}