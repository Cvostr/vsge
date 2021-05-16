#include "Serializer.hpp"
#include <fstream>

using namespace VSGE;
void Serializer::Reallocate(uint32 newSize) {
	byte* new_alloc = new byte[newSize];
	if (_bytes != nullptr) {
		memcpy(new_alloc, _bytes, _allocatedBytes);
		delete[] _bytes;
	}
	_bytes = new_alloc;
	_allocatedBytes = newSize;
}

void Serializer::WriteBytes(void* data, uint32 size) {
	if (_allocatedBytes < _writtenBytes + size) {
		Reallocate(_allocatedBytes * 2);
	}
	memcpy(_bytes + _writtenBytes, data, size);
	_writtenBytes += size;
}

void Serializer::write(const std::string& name, VarType type, uint32 size, byte* data) {
	std::string header = "_vl";
	WriteBytes((byte*)header.c_str(), 3);
	WriteBytes((byte*)name.c_str(), name.size());
	char zero = '\0';
	WriteBytes(&zero, 1);
	WriteBytes(&type, 1);
	WriteBytes(data, size);
}

void Serializer::BeginHeader(const std::string& name) {
	std::string header = "Hd";
	WriteBytes((byte*)header.c_str(), 2);
	WriteBytes((byte*)name.c_str(), name.size());
	char zero = '\0';
	WriteBytes(&zero, 1);
}
void Serializer::EndHeader() {
	std::string header = "Ed\n";
	WriteBytes((byte*)header.c_str(), 3);
}
void Serializer::store(const std::string& fname) {
	std::ofstream stream(fname, std::ios::binary);
	
	stream.write((const char*)_bytes, _writtenBytes);

	stream.close();
}