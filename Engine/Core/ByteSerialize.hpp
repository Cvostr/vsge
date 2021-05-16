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

		void Reallocate(uint32 newSize);
	public:

		ByteSerialize();

		~ByteSerialize() {
			delete[] mOutputBytes;
		}
		/// <summary>
		/// Write data
		/// </summary>
		/// <param name="data">- pointer to data to write</param>
		/// <param name="size">- size of data to write</param>
		void WriteBytes(void* data, uint32 size);
		/// <summary>
		/// Write string
		/// </summary>
		/// <param name="str">- string to write</param>
		void Serialize(std::string str);

		template<typename T>
		void Serialize(T* data) {
			WriteBytes((void*)data, sizeof(T));
		}

		template<typename T>
		void Serialize(T& data) {
			WriteBytes((void*)&data, sizeof(T));
		}
		/// <summary>
		/// Created bytes to write
		/// </summary>
		/// <returns></returns>
		byte* GetBytes();
		/// <summary>
		/// Size of bytes to write
		/// </summary>
		/// <returns></returns>
		uint32 GetSerializedSize();

	};
}