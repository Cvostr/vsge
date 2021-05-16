#pragma once

#include "../VarTypes/Base.hpp"
#include <string>
#include "Serializer.hpp"

namespace VSGE {

	class Deserializer {
	private:
		uint32 _offset;
		uint32 _size;
		byte* _data;

		void Copy(byte* data, uint32 size);
		void Read(byte* data, uint32 size);
		byte* GetCurrentByte();
		std::string ReadNextStringToNull();

	public:
		Deserializer(byte* data, uint32 size) :
			_data(data),
			_size(size),
			_offset(0)
		{}

		Deserializer(const std::string& file) :
			_offset(0)
		{
			Load(file);
		}

		void Load(const std::string& file);

		std::string GetNextHeaderName();

		bool isHeaderEnd();

		bool End();

		template<typename T>
		T GetValueByName(const std::string& name) {

		}
	};
}