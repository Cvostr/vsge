#include "DeSerializer.hpp"
#include <fstream>

using namespace VSGE;

byte* Deserializer::GetCurrentByte() {
	if (_offset >= _size)
		return nullptr;
	return (byte*)(_data + _offset);
}

void Deserializer::Copy(byte* data, uint32 size) {
	std::memcpy(data, GetCurrentByte(), size);
}

void Deserializer::Read(byte* data, uint32 size) {
	std::memcpy(data, GetCurrentByte(), size);
	_offset += size;
}

std::string Deserializer::ReadNextStringToNull() {
	std::string result;
	byte* bt = _data + _offset;
	while (*bt != '\0') {
		result.push_back(*bt);
		bt++;
		_offset++;
	}
	_offset++;

	return result;
}

bool Deserializer::isHeaderEnd() {
	char test[3];
	Copy((byte*)test, 3);
	bool result = strcmp(test, "Ed") == 0 ? true : false;

	if (result)
		_offset += 3;

	return result;
}

std::string Deserializer::GetNextHeaderName() {
	while (*GetCurrentByte() != 'H' && *(GetCurrentByte() + 1) != 'd') {
		_offset += 1;
	}
	_offset += 2;
	std::string _name = ReadNextStringToNull();
	return _name;
}

void Deserializer::Load(const std::string& file) {
	std::ifstream stream(file, std::ios::binary | std::ios::ate);

	_size = (uint32)stream.tellg();
	stream.seekg(0);
	_data = new byte[_size];
	stream.read((char*)_data, _size);
	stream.close();
}

bool Deserializer::End() {
	return _offset >= _size;
}