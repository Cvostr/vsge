#include "MonoScript.hpp"
#include <mono/metadata/assembly.h>
#include "MonoScriptingLayer.hpp"

using namespace VSGE;

MonoScriptInstance::MonoScriptInstance() :
    _mono_class_desc(nullptr),
    _mono_class_instance(nullptr)
{

}
MonoScriptInstance::~MonoScriptInstance() {
    Release();
}

MonoMethodDesc* MonoScriptInstance::GetMethodDesc(const std::string& method) {
    std::string method_str = _class_name + ":" + method;
    return mono_method_desc_new(method_str.c_str(), NULL);
}

MonoMethod* MonoScriptInstance::GetMethod(const std::string& method) {
    MonoScriptBlob* blob = MonoScriptingLayer::Get()->GetScriptsBlob();

    //get description of method
    MonoMethodDesc* desc = GetMethodDesc(method);

    if (!desc)
        return nullptr;
    // Search the method in the image
    return blob->GetMethodByDescription(desc);
}

MonoObject* MonoScriptInstance::CallMethod(MonoMethod* method_desc, void** args){
    MonoObject* result = mono_runtime_invoke(method_desc, _mono_class_instance, args, nullptr);
    return result;
}

bool MonoScriptInstance::CreateClassByName(const std::string& class_name) {
    this->_class_name = class_name;
    MonoScriptBlob* blob = MonoScriptingLayer::Get()->GetScriptsBlob();
    _mono_class_desc = blob->GetClassDescription(class_name);
    _mono_class_instance = mono_object_new(MonoScriptingLayer::Get()->GetDomain(), _mono_class_desc);

    return true;
}

void MonoScriptInstance::CallDefaultConstructor() {
    mono_runtime_object_init(_mono_class_instance);
}

void MonoScriptInstance::SetValueToField(const std::string field, void* value) {
    MonoClassField* field_desc = mono_class_get_field_from_name(_mono_class_desc, field.c_str());
    if (field_desc) {
        mono_field_set_value(_mono_class_instance, field_desc, value);
    }
}

void* MonoScriptInstance::GetValueOfField(const std::string& field) {
    void* result = nullptr;
    MonoClassField* field_desc = mono_class_get_field_from_name(_mono_class_desc, field.c_str());
    mono_field_get_value(_mono_class_instance, field_desc, &result);
    return result;
}

void MonoScriptInstance::CallOnStart() {
    MonoMethod* method = GetMethod("OnStart()");
    CallMethod(method, nullptr);
}

void MonoScriptInstance::Release() {
    if (_mono_class_instance) {
        mono_free(_mono_class_instance);
        _mono_class_instance = nullptr;
    }
}

const std::string& MonoScriptInstance::GetClassName() {
    return _class_name;
}
MonoClass* MonoScriptInstance::GetClassDesc() {
	return _mono_class_desc;
}

MonoMethod* MonoScriptInstance::GetUpdateMethod() {
	return _update_method;
}