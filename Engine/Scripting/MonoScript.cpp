#include "MonoScript.hpp"
#include "MonoLayer.hpp"
#include <mono/metadata/assembly.h>

using namespace VSGE;

MonoScript::MonoScript() :
	_assembly(nullptr),
	_image(nullptr),
	_main_class(nullptr)
{

}

MonoScript::~MonoScript() {

}

MonoClass* MonoScript::GetMainClass() {
	return _main_class;
}

bool MonoScript::CreateFromSourceFile(const std::string& source_path) {
	MonoLayer* mono = MonoLayer::Get();
	mono->CompileFile(source_path, "D:\\test.dll");

	return true;
}

bool MonoScript::CreateFromBytes(const byte* data, uint32 size, const std::string& class_name) {
	MonoImageOpenStatus status;
	_image = mono_image_open_from_data((char*)data, size, true, &status);


	_assembly = mono_assembly_load_from(_image, "", &status);

	
	_main_class = mono_class_from_name(_image, "", class_name.c_str());



	return true;
}