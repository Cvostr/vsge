#include "MonoClassDesc.hpp"
#include <mono/metadata/attrdefs.h>
#include "MonoScriptingLayer.hpp"

using namespace VSGE;

MonoClass* MonoClassDesc::GetMonoClass() const {
	return _class;
}

const std::string& MonoClassDesc::GetName() const {
	return _name;
}

std::vector<MonoClassFieldDesc>& MonoClassDesc::GetFields() {
	return _fields;
}

MonoClassDesc::MonoClassDesc() {
	_class = nullptr;
}

void MonoClassDesc::Create(MonoClass* mono_class) {
	_class = mono_class;
	_name = std::string(mono_class_get_name(mono_class));
	_namespace = std::string(mono_class_get_namespace(mono_class));
	//get field descs
	MonoClassField* cur_field = nullptr;
	void* pIterator = 0;
	while (true)
	{
		cur_field = mono_class_get_fields(mono_class, &pIterator);
		if (cur_field == nullptr)
		{
			break;
		}
		MonoClassFieldDesc cf_desc(this, cur_field);
		_fields.push_back(cf_desc);
	}
}

MonoObject* MonoClassDesc::CreateObjectInstance() {
	return mono_object_new(MonoScriptingLayer::Get()->GetDomain(), _class);
}

MonoClassDesc::MonoClassDesc(MonoClass* mono_class) {
	Create(mono_class);
}

MonoClassDesc::~MonoClassDesc() {

}

const std::string& MonoClassFieldDesc::GetName() const {
	return _name;
}

MonoClassFieldVisibility MonoClassFieldDesc::GetVisibility() const {
	return _visibility;
}

MonoObject* MonoClassFieldDesc::GetValue(MonoObject* object) {
	return mono_field_get_value_object(MonoScriptingLayer::Get()->GetDomain(), _field, object);
}

MonoClassFieldDesc::MonoClassFieldDesc(MonoClassDesc* class_desc, MonoClassField* mono_field) {
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
	MonoObject* value = GetValue(temp_obj);

	std::string type_string = GetTypeName();
	_type = GetValueTypeFromString(type_string);
}

const std::string MonoClassFieldDesc::GetTypeName() const {
	return std::string(mono_class_get_name(_type_class));
}