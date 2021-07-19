#pragma once

#include "Base.hpp"
#include "Guid.hpp"
#include <Math/Vec2.hpp>
#include <Math/Vec3.hpp>
#include <Math/Vec4.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Color.hpp>

enum ValueType {
	VALUE_TYPE_INT32 = 1,
	VALUE_TYPE_BOOL,
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_DOUBLE,
	VALUE_TYPE_INT64,
	VALUE_TYPE_PTR,
	VALUE_TYPE_VEC2F,
	VALUE_TYPE_VEC2I,
	VALUE_TYPE_VEC3F,
	VALUE_TYPE_VEC3I,
	VALUE_TYPE_VEC4F,
	VALUE_TYPE_VEC4I,
	VALUE_TYPE_QUAT,
	VALUE_TYPE_COLOR,
	VALUE_TYPE_GUID
};

union MultitypeData{
	int _int;
	bool _bool;
	float _float;
	double _double;
	int64 ___int64;
	void* _ptr;
	Vec2 _vec2f;
	Vec2i _vec2i;
	Vec3 _vec3f;
	Vec3i _vec3i;
	Vec4 _vec4f;
	Vec4i _vec4i;
	Quat _quat;
	VSGE::Color _color;
	VSGE::Guid guid;

	MultitypeData() :
		guid(0, 0, 0, 0)
	{
		
	}

	void operator=(MultitypeData& data) {
		memcpy(this, &data, sizeof(MultitypeData));
	}
};

class MultitypeValue {
private:
	MultitypeData _data;
	ValueType _type;
public:

	MultitypeValue() {
		_type = VALUE_TYPE_INT32;
		memset(&_data, 0, sizeof(MultitypeData));
	}

	ValueType GetType() {
		return _type;
	}

	template<typename T>
	T& GetValue() {
		return *(T*)(&_data);
	}

	void* GetValuePtr() {
		return &_data;
	}

	void SetData(ValueType type, MultitypeData value){
		_type = type;
		_data = value;
	}

	void SetInt(int _int) {
		_data._int = _int;
		_type = VALUE_TYPE_INT32;
	}
	void SetBool(bool _bool) {
		_data._bool = _bool;
		_type = VALUE_TYPE_BOOL;
	}
	void SetFloat(float _float) {
		_data._float = _float;
		_type = VALUE_TYPE_FLOAT;
	}
	void SetDouble(double _double) {
		_data._double = _double;
		_type = VALUE_TYPE_DOUBLE;
	}
	void SetInt64(int64 ___int64) {
		_data.___int64 = ___int64;
		_type = VALUE_TYPE_INT64;
	}
	void SetPtr(void* _ptr) {
		_data._ptr = _ptr;
		_type = VALUE_TYPE_PTR;
	}
	void SetVec2f(const Vec2& _vec) {
		_data._vec2f = _vec;
		_type = VALUE_TYPE_VEC2F;
	}
	void SetVec2i(const Vec2i& _vec) {
		_data._vec2i = _vec;
		_type = VALUE_TYPE_VEC2I;
	}
	void SetVec3f(const Vec3& _vec) {
		_data._vec3f = _vec;
		_type = VALUE_TYPE_VEC3F;
	}
	void SetVec3i(const Vec3i& _vec) {
		_data._vec3i = _vec;
		_type = VALUE_TYPE_VEC3I;
	}
	void SetVec4f(const Vec4& _vec) {
		_data._vec4f = _vec;
		_type = VALUE_TYPE_VEC4F;
	}
	void SetVec4i(const Vec4i& _vec) {
		_data._vec4i = _vec;
		_type = VALUE_TYPE_VEC4I;
	}
	void SetQuat(const Quat& _quat) {
		_data._quat = _quat;
		_type = VALUE_TYPE_QUAT;
	}
	void SetColor(const VSGE::Color& _color) {
		_data._color = _color;
		_type = VALUE_TYPE_COLOR;
	}



	MultitypeValue(int _int) {
		_data._int = _int;
		_type = VALUE_TYPE_INT32;
	}
	MultitypeValue(bool _bool) {
		_data._bool = _bool;
		_type = VALUE_TYPE_BOOL;
	}
	MultitypeValue(float _float) {
		_data._float = _float;
		_type = VALUE_TYPE_FLOAT;
	}
	MultitypeValue(double _double) {
		_data._double = _double;
		_type = VALUE_TYPE_DOUBLE;
	}
	MultitypeValue(int64 ___int64) {
		_data.___int64 = ___int64;
		_type = VALUE_TYPE_INT64;
	}
	MultitypeValue(void* _ptr) {
		_data._ptr = _ptr;
		_type = VALUE_TYPE_PTR;
	}
	MultitypeValue(const Vec2& _vec) {
		_data._vec2f = _vec;
		_type = VALUE_TYPE_VEC2F;
	}
	MultitypeValue(const Vec2i& _vec) {
		_data._vec2i = _vec;
		_type = VALUE_TYPE_VEC2I;
	}
	MultitypeValue(const Vec3& _vec) {
		_data._vec3f = _vec;
		_type = VALUE_TYPE_VEC3F;
	}
	MultitypeValue(const Vec3i& _vec) {
		_data._vec3i = _vec;
		_type = VALUE_TYPE_VEC3I;
	}
	MultitypeValue(const Vec4& _vec) {
		_data._vec4f = _vec;
		_type = VALUE_TYPE_VEC4F;
	}
	MultitypeValue(const Vec4i& _vec) {
		_data._vec4i = _vec;
		_type = VALUE_TYPE_VEC4I;
	}
	MultitypeValue(const Quat& _quat) {
		_data._quat = _quat;
		_type = VALUE_TYPE_QUAT;
	}
	MultitypeValue(const VSGE::Color& _color) {
		_data._color = _color;
		_type = VALUE_TYPE_COLOR;
	}
};
