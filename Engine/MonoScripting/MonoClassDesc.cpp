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

std::vector<MonoClassFieldDesc*>& MonoClassDesc::GetFields() {
	return _fields;
}

std::vector<MonoEventDesc*>& MonoClassDesc::GetEvents() {
	return _events;
}

std::vector<MonoMethodDescr*>& MonoClassDesc::GetMethods() {
	return _methods;
}

MonoMethodDescr* MonoClassDesc::GetMethodDescByName(const std::string& method_name) {
	for (auto method_desc : _methods) {
		if (method_desc->GetName() == method_name) {
			return method_desc;
		}
	}
	return nullptr;
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
	void* iterator = nullptr;
	while (true)
	{
		cur_field = mono_class_get_fields(mono_class, &iterator);
		if (cur_field == nullptr)
		{
			break;
		}
		MonoClassFieldDesc* cf_desc = new MonoClassFieldDesc(this, cur_field);
		_fields.push_back(cf_desc);
	}
	iterator = nullptr;
	MonoEvent* event = nullptr;
	while (event = mono_class_get_events(mono_class, &iterator))
	{
		MonoEventDesc* ev_desc = new MonoEventDesc(this, event);
		_events.push_back(ev_desc);
	}

	iterator = nullptr;
	MonoMethod* method = nullptr;
	while (method = mono_class_get_methods(mono_class, &iterator)) 
	{
		MonoMethodDescr* m_desc = new MonoMethodDescr(this, method);
		_methods.push_back(m_desc);
	}
}

MonoObject* MonoClassDesc::CreateObjectInstance() {
	return mono_object_new(MonoScriptingLayer::Get()->GetDomain(), _class);
}

MonoClassDesc::MonoClassDesc(MonoClass* mono_class) {
	Create(mono_class);
}

MonoClassDesc::~MonoClassDesc() {
	for (uint32 i = 0; i < _fields.size(); i++) {
		delete _fields[i];
	}
	for (uint32 i = 0; i < _methods.size(); i ++) {
		delete _methods[i];
	}
	for (uint32 i = 0; i < _events.size(); i++) {
		delete _events[i];
	}
}