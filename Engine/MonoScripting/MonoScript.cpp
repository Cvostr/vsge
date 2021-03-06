#include "MonoScript.hpp"
#include <mono/metadata/assembly.h>
#include "MonoScriptingLayer.hpp"

using namespace VSGE;

MonoScriptInstance::MonoScriptInstance() :
    _mono_class_desc(nullptr),
    _mono_class_instance(nullptr),
    _update_method(nullptr)
{

}
MonoScriptInstance::~MonoScriptInstance() {
    Release();
}

MonoMethodDesc* MonoScriptInstance::GetMethodDesc(const std::string& method) {
    std::string method_str = _class_name + ":" + method;
    return mono_method_desc_new(method_str.c_str(), false);
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
    _mono_class_desc = blob->GetMonoClassDesc(class_name);
    _mono_class_instance = _mono_class_desc->CreateObjectInstance();

    _update_method = GetMethod("OnUpdate()");

    return true;
}

void MonoScriptInstance::CallDefaultConstructor() {
    mono_runtime_object_init(_mono_class_instance);
}

void MonoScriptInstance::SetValuePtrToField(const std::string field, void* value) {
    MonoClassField* field_desc = mono_class_get_field_from_name(_mono_class_desc->GetMonoClass(), field.c_str());
    if (field_desc) {
        mono_field_set_value(_mono_class_instance, field_desc, value);
    }
}

void MonoScriptInstance::SetStringToField(const std::string field, const String& str) {
    MonoString* mono_str = mono_string_new_utf32(MonoScriptingLayer::Get()->GetDomain(), str.c_str(), str.Length());
    SetValuePtrToField(field, mono_str);
}

void MonoScriptInstance::SetStringToField(const std::string field, const std::string& str) {
    MonoString* mono_str = mono_string_new(MonoScriptingLayer::Get()->GetDomain(), str.c_str());
    SetValuePtrToField(field, mono_str);
}

void* MonoScriptInstance::GetValueOfField(const std::string& field) {
    void* result = nullptr;
    MonoClassField* field_desc = mono_class_get_field_from_name(_mono_class_desc->GetMonoClass(), field.c_str());
    mono_field_get_value(_mono_class_instance, field_desc, &result);
    return result;
}

void MonoScriptInstance::CallOnStart() {
    MonoMethod* method = GetMethod("OnStart()");
    if(method)
        CallMethod(method, nullptr);
}

void MonoScriptInstance::CallOnUpdate() {
    if (_update_method)
        CallMethod(_update_method, nullptr);
}

void MonoScriptInstance::CallOnTriggerStay(void* entity) {
   
}
void MonoScriptInstance::CallOnTriggerEnter(void* entity) {

}
void MonoScriptInstance::CallOnTriggerExit(void* entity) {

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
	return _mono_class_desc->GetMonoClass();
}
MonoObject* MonoScriptInstance::GetObject() {
    return _mono_class_instance;
}
MonoMethod* MonoScriptInstance::GetUpdateMethod() {
	return _update_method;
}