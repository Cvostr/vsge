#include "MonoScript.hpp"
#include <mono/metadata/assembly.h>
#include "MonoScriptingLayer.hpp"

using namespace VSGE;

MonoScriptInstance::MonoScriptInstance() :
    _mono_class(nullptr)
{

}
MonoScriptInstance::~MonoScriptInstance() {

}

MonoMethod* MonoScriptInstance::GetMethod(const std::string& method) {
    MonoScriptBlob* blob = MonoScriptingLayer::Get()->GetScriptsBlob();

    // Get method description
    MonoMethodDesc* mono_method_desc = mono_method_desc_new(method.c_str(), NULL);
    if (!mono_method_desc)
    {
        return nullptr;
    }

    // Search the method in the image
    MonoMethod* mono_method = mono_method_desc_search_in_image(mono_method_desc, blob->GetImage());
    if (!mono_method)
    {
        return nullptr;
    }

    return mono_method;
}
MonoClass* MonoScriptInstance::GetMainClass() {
	return _mono_class;
}
MonoMethod* MonoScriptInstance::GetStartMethod() {
	return _start_method;
}
MonoMethod* MonoScriptInstance::GetUpdateMethod() {
	return _update_method;
}