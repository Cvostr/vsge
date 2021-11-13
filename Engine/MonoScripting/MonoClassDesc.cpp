#include "MonoClassDesc.hpp"
#include "MonoScriptingLayer.hpp"

using namespace VSGE;

MonoClass* MonoClassDesc::GetMonoClass() const {
	return _class;
}

const std::string& MonoClassDesc::GetName() const {
	return _name;
}

const std::string& MonoClassDesc::GetNamespace() const {
	return _namespace;
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