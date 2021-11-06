#include "Variant.hpp"
#include "string.h"

MultitypeData::MultitypeData()
{
	memset(this, 0, sizeof(MultitypeData));
}

void MultitypeData::operator=(MultitypeData& data) {
	memcpy(this, &data, sizeof(MultitypeData));
}

Variant::Variant() {
	_type = VALUE_TYPE_INT32;
	memset(&_data, 0, sizeof(MultitypeData));
}

ValueType Variant::GetType() {
	return _type;
}

void* Variant::GetValuePtr() {
	return &_data;
}

void Variant::SetData(ValueType type, MultitypeData value) {
	_type = type;
	_data = value;
}

void Variant::SetInt(int _int) {
	_data._int = _int;
	_type = VALUE_TYPE_INT32;
}
void Variant::SetBool(bool _bool) {
	_data._bool = _bool;
	_type = VALUE_TYPE_BOOL;
}
void Variant::SetFloat(float _float) {
	_data._float = _float;
	_type = VALUE_TYPE_FLOAT;
}
void Variant::SetDouble(double _double) {
	_data._double = _double;
	_type = VALUE_TYPE_DOUBLE;
}
void Variant::SetInt64(int64 ___int64) {
	_data.___int64 = ___int64;
	_type = VALUE_TYPE_INT64;
}
void Variant::SetPtr(void* _ptr) {
	_data._ptr = _ptr;
	_type = VALUE_TYPE_PTR;
}
void Variant::SetVec2f(const Vec2& _vec) {
	_data._vec2f = _vec;
	_type = VALUE_TYPE_VEC2F;
}
void Variant::SetVec2i(const Vec2i& _vec) {
	_data._vec2i = _vec;
	_type = VALUE_TYPE_VEC2I;
}
void Variant::SetVec3f(const Vec3& _vec) {
	_data._vec3f = _vec;
	_type = VALUE_TYPE_VEC3F;
}
void Variant::SetVec3i(const Vec3i& _vec) {
	_data._vec3i = _vec;
	_type = VALUE_TYPE_VEC3I;
}
void Variant::SetVec4f(const Vec4& _vec) {
	_data._vec4f = _vec;
	_type = VALUE_TYPE_VEC4F;
}
void Variant::SetVec4i(const Vec4i& _vec) {
	_data._vec4i = _vec;
	_type = VALUE_TYPE_VEC4I;
}
void Variant::SetQuat(const Quat& _quat) {
	_data._quat = _quat;
	_type = VALUE_TYPE_QUAT;
}
void Variant::SetColor(const VSGE::Color& _color) {
	_data._color = _color;
	_type = VALUE_TYPE_COLOR;
}

ValueType GetValueTypeFromString(const std::string& type_str) {
	if (type_str == "Int32")
		return VALUE_TYPE_INT32;
	else if (type_str == "String")
		return VALUE_TYPE_STRING;
	else if (type_str == "Vec3")
		return VALUE_TYPE_VEC3F;
	else if (type_str == "Vec2")
		return VALUE_TYPE_VEC2F;
}