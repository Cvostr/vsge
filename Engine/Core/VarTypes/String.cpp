#include "String.hpp"
#include <string.h>

String::String() : _data(nullptr), _length(0) {}

String::String(char* str, uint32 len) {
	_data = nullptr;
	_length = 0;
}
String::String(Char* str, uint32 len) {
	_data = nullptr;
	_length = 0;
	if (len > 0) {
		_length = len;
		_data = new Char[_length];
		memcpy(_data, str, len);
	}else if (len == 0) {
		//find length of input string
		uint32 length = 0;
		while (str[length] != 0) {
			length++;
		}
		//allocate memory for string
		_data = new Char[length];

		for (uint32 i = 0; i < length; i++) {
			_data[i] = str[i];
		}
	}
}
String::String(const std::string& str) {
	FromStdString(str);
}
String::~String() {
	SAFE_RELEASE_ARR(_data)
}

void String::Clear() {
	SAFE_RELEASE_ARR(_data);
	_length = 0;
}

uint32 String::Length() const {
	return _length;
}

bool String::IsEmpty() const {
	return _length == 0;
}

const Char* String::c_str() const {
	return _data;
}

Char& String::operator[](uint32 i) const {
	return _data[i];
}

void String::operator=(const String& str) {
	Set(str);
}

bool String::operator==(const String& str) const {
	if (_length != str.Length())
		return false;
	else {
		for (uint32 i = 0; i < _length; i++) {
			if (_data[i] != str[i])
				return false;
		}
	}
	return true;
}

void String::FromStdString(const std::string& str) {
	Clear();
	_data = new Char[str.size()];
	_length = str.size();

	for (uint32 i = 0; i < _length; i++) {
		_data[i] = (Char)str.at(i);
	}
}

std::string String::GetStdString() const {
	std::string result;
	for (uint32 i = 0; i < _length; i++) {
		result.push_back((char)_data[i]);
	}
	return result;
}

void String::Resize(uint32 new_length) {
	Char* new_data = new Char[new_length];
	uint32 copy_len = (_length > new_length) ? new_length : _length;

	for (uint32 i = 0; i < copy_len; i++) {
		new_data[i] = _data[i];
	}
	
	_length = new_length;
	SAFE_RELEASE_ARR(_data);
	_data = new_data;
}

void String::Add(Char symbol) {
	Resize(_length + 1);
	_data[_length] = symbol;
}

void String::PopBack() {
	Resize(_length - 1);
}

void String::Set(const String& str) {
	Resize(str.Length());
	for (uint32 i = 0; i < str.Length(); i++) {
		_data[i] = str.c_str()[i];
	}
}