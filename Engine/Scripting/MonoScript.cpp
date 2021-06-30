#include "MonoScript.hpp"
#include "MonoLayer.hpp"
#include <mono/metadata/assembly.h>

using namespace VSGE;

MonoScript::MonoScript() :
	_assembly(nullptr),
	_image(nullptr),
	_main_class(nullptr),

	_start_method(nullptr),
	_update_method(nullptr)
{

}

MonoScript::~MonoScript() {

}

MonoClass* MonoScript::GetMainClass() {
	return _main_class;
}

bool MonoScript::CreateFromSourceFile(const std::string& source_path) {
	MonoLayer* mono = MonoLayer::Get();
	//mono->CompileFile(source_path, "D:\\test.dll");

	return true;
}

bool MonoScript::CreateFromBytes(const byte* data, uint32 size, const std::string& class_name) {
	MonoImageOpenStatus status;
	_image = mono_image_open_from_data((char*)data, size, true, &status);


	_assembly = mono_assembly_load_from(_image, "", &status);

	
	_main_class = mono_class_from_name(_image, "", class_name.c_str());

	_start_method = GetMethod(class_name + ":Start()");
	_update_method = GetMethod(class_name + ":Update(single)");

	return true;
}

MonoMethod* MonoScript::GetStartMethod() {
	return _start_method;

}
MonoMethod* MonoScript::GetUpdateMethod() {
	return _update_method;
}

MonoMethod* MonoScript::GetMethod(const std::string& method)
{
    // Get method description
    MonoMethodDesc* mono_method_desc = mono_method_desc_new(method.c_str(), NULL);
    if (!mono_method_desc)
    {
        return nullptr;
    }

    // Search the method in the image
    MonoMethod* mono_method = mono_method_desc_search_in_image(mono_method_desc, _image);
    if (!mono_method)
    {
        return nullptr;
    }

    return mono_method;
}