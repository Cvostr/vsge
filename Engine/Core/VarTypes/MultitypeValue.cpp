#include "MultitypeValue.hpp"
#include "string.h"

MultitypeData::MultitypeData()
{
	memset(this, 0, sizeof(MultitypeData));
}

void MultitypeData::operator=(MultitypeData& data) {
	memcpy(this, &data, sizeof(MultitypeData));
}

MultitypeValue::MultitypeValue() {
	_type = VALUE_TYPE_INT32;
	memset(&_data, 0, sizeof(MultitypeData));
}

ValueType MultitypeValue::GetType() {
	return _type;
}

void* MultitypeValue::GetValuePtr() {
	return &_data;
}

void MultitypeValue::SetData(ValueType type, MultitypeData value) {
	_type = type;
	_data = value;
}

void MultitypeValue::SetInt(int _int) {
	_data._int = _int;
	_type = VALUE_TYPE_INT32;
}
void MultitypeValue::SetBool(bool _bool) {
	_data._bool = _bool;
	_type = VALUE_TYPE_BOOL;
}
void MultitypeValue::SetFloat(float _float) {
	_data._float = _float;
	_type = VALUE_TYPE_FLOAT;
}
void MultitypeValue::SetDouble(double _double) {
	_data._double = _double;
	_type = VALUE_TYPE_DOUBLE;
}
void MultitypeValue::SetInt64(int64 ___int64) {
	_data.___int64 = ___int64;
	_type = VALUE_TYPE_INT64;
}
void MultitypeValue::SetPtr(void* _ptr) {
	_data._ptr = _ptr;
	_type = VALUE_TYPE_PTR;
}
void MultitypeValue::SetVec2f(const Vec2& _vec) {
	_data._vec2f = _vec;
	_type = VALUE_TYPE_VEC2F;
}
void MultitypeValue::SetVec2i(const Vec2i& _vec) {
	_data._vec2i = _vec;
	_type = VALUE_TYPE_VEC2I;
}
void MultitypeValue::SetVec3f(const Vec3& _vec) {
	_data._vec3f = _vec;
	_type = VALUE_TYPE_VEC3F;
}
void MultitypeValue::SetVec3i(const Vec3i& _vec) {
	_data._vec3i = _vec;
	_type = VALUE_TYPE_VEC3I;
}
void MultitypeValue::SetVec4f(const Vec4& _vec) {
	_data._vec4f = _vec;
	_type = VALUE_TYPE_VEC4F;
}
void MultitypeValue::SetVec4i(const Vec4i& _vec) {
	_data._vec4i = _vec;
	_type = VALUE_TYPE_VEC4I;
}
void MultitypeValue::SetQuat(const Quat& _quat) {
	_data._quat = _quat;
	_type = VALUE_TYPE_QUAT;
}
void MultitypeValue::SetColor(const VSGE::Color& _color) {
	_data._color = _color;
	_type = VALUE_TYPE_COLOR;
}