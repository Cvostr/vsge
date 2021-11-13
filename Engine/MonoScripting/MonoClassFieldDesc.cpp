#include "MonoClassFieldDesc.hpp"
#include "MonoScriptingLayer.hpp"
#include <mono/metadata/attrdefs.h>

using namespace VSGE;

const std::string& MonoClassFieldDesc::GetName() const {
	return _name;
}

MonoClassFieldVisibility MonoClassFieldDesc::GetVisibility() const {
	return _visibility;
}

MonoClassDesc* MonoClassFieldDesc::GetClass() const {
	return _class;
}

MonoObject* MonoClassFieldDesc::GetValueMonoObject(MonoObject* object) {
	return mono_field_get_value_object(MonoScriptingLayer::Get()->GetDomain(), _field, object);
}

ValueType MonoClassFieldDesc::GetValueType() {
	std::string type_string = GetTypeName();

	if (type_string == "Int32")
		return VALUE_TYPE_INT32;
	else if (type_string == "Boolean")
		return VALUE_TYPE_BOOL;
	else if (type_string == "Float")
		return VALUE_TYPE_FLOAT;
	else if (type_string == "double")
		return VALUE_TYPE_DOUBLE;
	else if (type_string == "String")
		return VALUE_TYPE_STRING;
	else if (type_string == "Vec3")
		return VALUE_TYPE_VEC3F;
	else if (type_string == "Vec2")
		return VALUE_TYPE_VEC2F;
	else if (type_string == "Vec4")
		return VALUE_TYPE_VEC4F;
	else if (type_string == "Quat")
		return VALUE_TYPE_QUAT;
	else if (type_string == "Color")
		return VALUE_TYPE_COLOR;

	return VALUE_TYPE_INT32;
}

Variant MonoClassFieldDesc::GetValue(MonoObject* object) {
	MonoObject* value = GetValueMonoObject(object);

	ValueType type = GetValueType();
	_base_value.SetType(type);

	return _base_value;
}

void MonoClassFieldDesc::Create(MonoClassDesc* class_desc, MonoClassField* mono_field) {
	_field = mono_field;
	_class = class_desc;
	_name = std::string(mono_field_get_name(mono_field));

	MonoType* field_type = mono_field_get_type(mono_field);
	_type_class = (mono_class_from_mono_type(field_type));

	int visiblity = mono_field_get_flags(mono_field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

	switch (visiblity) {
	case MONO_FIELD_ATTR_PRIVATE:
		_visibility = FIELD_PRIVATE;
		break;
	case MONO_FIELD_ATTR_ASSEMBLY:
		_visibility = FIELD_INTERNAL;
		break;
	case MONO_FIELD_ATTR_FAMILY:
		_visibility = FIELD_PROTECTED;
		break;
	case MONO_FIELD_ATTR_PUBLIC:
		_visibility = FIELD_PUBLIC;
		break;
	}

	MonoObject* temp_obj = _class->CreateObjectInstance();
	_base_value = GetValue(temp_obj);
}

MonoClassFieldDesc::MonoClassFieldDesc(MonoClassDesc* class_desc, MonoClassField* mono_field) {
	Create(class_desc, mono_field);
}

const std::string MonoClassFieldDesc::GetTypeName() const {
	return std::string(mono_class_get_name(_type_class));
}