#include "MonoScriptBlob.hpp"
#include "MonoScriptingLayer.hpp"

using namespace VSGE;

MonoScriptBlob::MonoScriptBlob() :
	_assembly(nullptr),
	_image(nullptr)
{

}
MonoScriptBlob::~MonoScriptBlob() {

}

MonoAssembly* MonoScriptBlob::GetAssembly() {
	return _assembly;
}
MonoImage* MonoScriptBlob::GetImage() {
	return _image;
}

void MonoScriptBlob::LoadFromFile(const std::string& path) {
	_assembly = mono_domain_assembly_open(MonoScriptingLayer::Get()->GetDomain(), path.c_str());
	_image = mono_assembly_get_image(_assembly);
}

MonoClass* MonoScriptBlob::GetClass(const std::string& class_name) {
	return mono_class_from_name(_image, "", class_name.c_str());
}