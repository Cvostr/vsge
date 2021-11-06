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

void MonoScriptBlob::Release() {
	if (_assembly) {
		mono_assembly_close(_assembly);
		//mono_free(_assembly);
		_assembly = nullptr;
	}
	if (_image) {
		//mono_free(_image);
		//_image = nullptr;
	}
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
	BuildMonoClassDescsList();
}

MonoClass* MonoScriptBlob::GetClassDescription(const std::string& class_name, const std::string& namespace_name) {
	return mono_class_from_name(_image, namespace_name.c_str(), class_name.c_str());
}

MonoMethod* MonoScriptBlob::GetMethodByDescription(MonoMethodDesc* method_desc) {
	return mono_method_desc_search_in_image(method_desc, _image);
}

void MonoScriptBlob::BuildMonoClassDescsList() {
	_class_descs.clear();

	const MonoTableInfo* table_info = mono_image_get_table_info(_image, MONO_TABLE_TYPEDEF);

	int rows = mono_table_info_get_rows(table_info);

	for (int i = 0; i < rows; i++)
	{
		MonoClass* _class = nullptr;
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(_image, cols[MONO_TYPEDEF_NAME]);
		const char* name_space = mono_metadata_string_heap(_image, cols[MONO_TYPEDEF_NAMESPACE]);
		_class = mono_class_from_name(_image, name_space, name);
		
		MonoClassDesc desc(_class);
		_class_descs.push_back(desc);
	}
}