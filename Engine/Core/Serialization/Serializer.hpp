#pragma once

#include "../VarTypes/Base.hpp"
#include "../VarTypes/Guid.hpp"
#include <string>

namespace VSGE {

	enum VarType {
		VAR_TYPE_INT,
		VAR_TYPE_FLOAT,
		VAR_TYPE_VEC3,
		VAR_TYPE_STRING,
		VAR_TYPE_BOOL,
		VAR_TYPE_GUID
	};

	class Serializer {
	private:
		uint32 _writtenBytes;
		uint32 _allocatedBytes;

		byte* _bytes;

		void Reallocate(uint32 newSize);
		void WriteBytes(void* data, uint32 size);
	public:

		Serializer() :
			_writtenBytes(0),
			_bytes(nullptr),
			_allocatedBytes(0)
		{
			Reallocate(1000);
		}

		void store(const std::string& fname);

		void write(const std::string& name, VarType type, uint32 size, byte* data);

		template<typename T>
		void Serialize(const std::string& name, T& value) {
			if (typeid(T) == typeid(std::string)) {
				std::string* s = (std::string*)(&value);
				write(name, VAR_TYPE_STRING, s->size(), (byte*)s->c_str());
				char zero = '\0';
				WriteBytes(&zero, 1);
			}
			if (typeid(T) == typeid(int)) {
				write(name, VAR_TYPE_INT, 4, (byte*)&value);
			}
			if (typeid(T) == typeid(float)) {
				write(name, VAR_TYPE_FLOAT, 4, (byte*)&value);
			}
			if (typeid(T) == typeid(bool)) {
				write(name, VAR_TYPE_BOOL, 1, (byte*)&value);
			}
			if (typeid(T) == typeid(Guid)) {
				write(name, VAR_TYPE_GUID, sizeof(Guid), (byte*)&((Guid*)&value)->Values);
			}
		}

		void BeginHeader(const std::string& name);
		void EndHeader();
	};
}