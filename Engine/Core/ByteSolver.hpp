#pragma once

#include "VarTypes/Base.hpp"
#include <string>

namespace VSGE {
	class ByteSolver {
	private:
		byte* mBytes;
		uint64 mSize;
		uint32 mOffset;

	public:
		ByteSolver(byte* bytes, uint64 size = UINT64_MAX) :
			mBytes(bytes),
			mOffset(0),
			mSize(size)
		{}

		void Copy(void* dest, uint32 size);

		std::string ReadNextString();

		bool end() {
			return mOffset >= mSize;
		}

		void move(uint32 offset) {
			mOffset += offset;
		}

		template<typename T>
		void Copy(T* dest) {
			Copy(dest, sizeof(T));
		}

		template<typename T>
		void Copy(T& dest) {
			Copy(&dest, sizeof(T));
		}

		template<typename T>
		T GetValue() {
			T value;
			Copy(&value, sizeof(T));
			return value;
		}

	};
}