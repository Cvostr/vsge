#include "Memory.hpp"

void Memcpy(void* dst, const void* src, uint64 size) {
	byte* dst_ptr = (byte*)dst;
	const byte* src_ptr = (const byte*)src;
	for (uint64 i = 0; i < size; i++)
		dst_ptr[i] = src_ptr[i];
}

int Memcmp(const void* a, void* b, uint64 size) {
	const byte* a_ptr = (const byte*)a;
	const byte* b_ptr = (const byte*)b;
	for (size_t i = 0; i < size; i++) {
		if (a_ptr[i] < b_ptr[i])
			return -1;
		else if (b_ptr[i] < a_ptr[i])
			return 1;
	}
	return 0;
}