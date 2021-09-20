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

	MultitypeData();

	void operator=(MultitypeData& data);
};

class Variant {
private:
	MultitypeData _data;
	ValueType _type;
public:

	Variant();

	ValueType GetType();

	template<typename T>
	T& GetValue() {
		return *(T*)(&_data);
	}

	void* GetValuePtr();

	void SetData(ValueType type, MultitypeData value);
	void SetInt(int _int);
	void SetBool(bool _bool);
	void SetFloat(float _float);
	void SetDouble(double _double);
	void SetInt64(int64 ___int64);
	void SetPtr(void* _ptr);
	void SetVec2f(const Vec2& _vec);
	void SetVec2i(const Vec2i& _vec);
	void SetVec3f(const Vec3& _vec);
	void SetVec3i(const Vec3i& _vec);
	void SetVec4f(const Vec4& _vec);
	void SetVec4i(const Vec4i& _vec);
	void SetQuat(const Quat& _quat);
	void SetColor(const VSGE::Color& _color);


	Variant(int _int) {
		_data._int = _int;
		_type = VALUE_TYPE_INT32;
	}
	Variant(bool _bool) {
		_data._bool = _bool;
		_type = VALUE_TYPE_BOOL;
	}
	Variant(float _float) {
		_data._float = _float;
		_type = VALUE_TYPE_FLOAT;
	}
	Variant(double _double) {
		_data._double = _double;
		_type = VALUE_TYPE_DOUBLE;
	}
	Variant(int64 ___int64) {
		_data.___int64 = ___int64;
		_type = VALUE_TYPE_INT64;
	}
	Variant(void* _ptr) {
		_data._ptr = _ptr;
		_type = VALUE_TYPE_PTR;
	}
	Variant(const Vec2& _vec) {
		_data._vec2f = _vec;
		_type = VALUE_TYPE_VEC2F;
	}
	Variant(const Vec2i& _vec) {
		_data._vec2i = _vec;
		_type = VALUE_TYPE_VEC2I;
	}
	Variant(const Vec3& _vec) {
		_data._vec3f = _vec;
		_type = VALUE_TYPE_VEC3F;
	}
	Variant(const Vec3i& _vec) {
		_data._vec3i = _vec;
		_type = VALUE_TYPE_VEC3I;
	}
	Variant(const Vec4& _vec) {
		_data._vec4f = _vec;
		_type = VALUE_TYPE_VEC4F;
	}
	Variant(const Vec4i& _vec) {
		_data._vec4i = _vec;
		_type = VALUE_TYPE_VEC4I;
	}
	Variant(const Quat& _quat) {
		_data._quat = _quat;
		_type = VALUE_TYPE_QUAT;
	}
	Variant(const VSGE::Color& _color) {
		_data._color = _color;
		_type = VALUE_TYPE_COLOR;
	}
};
