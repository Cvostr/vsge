#include "Variant.hpp"
#include "string.h"

MultitypeData::MultitypeData()
{
	memset(this, 0, sizeof(MultitypeData));
}

MultitypeData::~MultitypeData()
{
	
}

void MultitypeData::Assign(MultitypeData& data) {
	memcpy(this, &data, sizeof(MultitypeData));
}

void MultitypeData::operator=(MultitypeData& data) {
	Assign(data);
}

Variant::Variant() {
	_type = VALUE_TYPE_INT32;
	memset(&_data, 0, sizeof(MultitypeData));
}

ValueType Variant::GetType() const {
	return _type;
}

void Variant::SetType(ValueType type) {
	_type = type;
}

void* Variant::GetValuePtr() {
	return &_data;
}

const MultitypeData& Variant::GetMultitypeData() const{
	return _data;
}

void Variant::SetData(ValueType type, MultitypeData value) {
	_type = type;
	_data = value;
}

void Variant::operator=(const Variant& v){
	SetData(v.GetType(), v.GetMultitypeData());
}