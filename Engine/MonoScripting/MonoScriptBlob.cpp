#include "MonoScriptBlob.hpp"
#include "MonoScriptingLayer.hpp"

using namespace VSGE;

MonoScriptBlob::MonoScriptBlob() :
	_assembly(nullptr),
	_image(nullptr)
{

}
MonoScriptBlob::~MonoScriptBlob() {
	ClearDescs();
}

void MonoScriptBlob::ClearDescs() {
	for (auto class_desc : _class_descs) {
		delete class_desc;
	}
	_class_descs.clear();
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

MonoClassDesc* MonoScriptBlob::GetMonoClassDesc(const std::string& class_name, const std::string& namespace_name) {
	for (auto& desc : _class_descs) {
		if (desc->GetName() == class_name)
			return desc;
	}
	return nullptr;
}

void MonoScriptBlob::LoadFromFile(const std::string& path) {
	_assembly = mono_domain_assembly_open(MonoScriptingLayer::Get()->GetDomain(), path.c_str());
	_image = mono_assembly_get_image(_assembly);
	BuildMonoClassDescsList();
}

MonoMethod* MonoScriptBlob::GetMethodByDescription(MonoMethodDesc* method_desc) {
	return mono_method_desc_search_in_image(method_desc, _image);
}

void MonoScriptBlob::BuildMonoClassDescsList() {
	ClearDescs();

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
		
		MonoClassDesc* desc = new MonoClassDesc(_class);
		_class_descs.push_back(desc);
	}
}