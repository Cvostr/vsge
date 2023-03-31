#pragma once

#include "VarTypes/Base.hpp"
#include <string>
#include <mpi/Core/Guid.hpp>

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

		bool end() const;

		void move(uint32 offset);

		Guid GetGuid();

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