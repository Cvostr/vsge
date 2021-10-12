#pragma once

#include <Core/VarTypes/Base.hpp>
#include <string>

typedef unsigned int Char;

class String {
private:
	Char* _data;
	uint32 _length;
public:

	String();
	String(char* str, uint32 len = 0);
	String(Char* str, uint32 len = 0);
	String(const std::string& str);
	~String();
	
	void Clear();

	uint32 Length() const ;

	bool IsEmpty() const;

	const Char* c_str() const;

	Char& operator[](uint32 i);

	void operator=(const String& str);

	bool operator==(const String& str) const;
	
	void FromStdString(const std::string& str);

	std::string GetStdString() const;

	void Resize(uint32 new_length);

	void Add(Char symbol);

	void PopBack();

	void Set(const String& str);
};