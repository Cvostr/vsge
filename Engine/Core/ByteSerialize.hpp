#pragma once

#include "ByteSerialize.hpp"
#include <Core/VarTypes/Base.hpp>
#include <string>

namespace VSGE {
	class ByteSerialize {
	private:
		byte* mOutputBytes;
		uint32 mAllocatedSize;
		uint32 mSerializedSize;
	public:

		ByteSerialize();

		~ByteSerialize() {
			delete[] mOutputBytes;
		}

		void Reallocate(uint32 newSize);

		void Serialize(void* data, uint32 size);

		void Serialize(std::string str);

		template<typename T>
		void Serialize(T* data) {
			Serialize((void*)data, sizeof(T));
		}

		template<typename T>
		void Serialize(T& data) {
			Serialize((void*)&data, sizeof(T));
		}

		byte* GetBytes();

		uint32 GetSerializedSize();

	};
}