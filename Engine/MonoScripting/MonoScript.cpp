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
    return mono_method_desc_new(method.c_str(), NULL);
}

MonoMethod* MonoScriptInstance::GetMethod(const std::string& method) {
    MonoScriptBlob* blob = MonoScriptingLayer::Get()->GetScriptsBlob();

    MonoMethodDesc* desc = GetMethodDesc(method);

    // Search the method in the image
    return blob->GetMethodByDescription(desc);
}

MonoObject* MonoScriptInstance::CallMethod(MonoMethod* method_desc, void** args){
    MonoObject* result = mono_runtime_invoke(method_desc, NULL, args, &_mono_class_instance);
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
MonoMethod* MonoScriptInstance::GetStartMethod() {
	return _start_method;
}
MonoMethod* MonoScriptInstance::GetUpdateMethod() {
	return _update_method;
}