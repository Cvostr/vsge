#include "MonoApiDescriptions.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;
using namespace VSGEditor;

MonoApiDescriptions::MonoApiDescriptions() {
	_assembly = nullptr;
	_image = nullptr;
}

MonoApiDescriptions::~MonoApiDescriptions() {

}

void MonoApiDescriptions::Load() {
	_assembly = mono_domain_assembly_open(MonoScriptingLayer::Get()->GetDomain(), "api.dll");
	_image = mono_assembly_get_image(_assembly);

	const MonoTableInfo* table_info = mono_image_get_table_info(_image, MONO_TABLE_TYPEDEF);
	//get count of c# classes
	int rows = mono_table_info_get_rows(table_info);

	for (int i = 0; i < rows; i++)
	{
		MonoClass* _class = nullptr;
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(_image, cols[MONO_TYPEDEF_NAME]);
		const char* name_space = mono_metadata_string_heap(_image, cols[MONO_TYPEDEF_NAMESPACE]);
		_class = mono_class_from_name(_image, name_space, name);
		if (_class) {
			MonoClassDesc* desc = new MonoClassDesc(_class);
			_api_class_descs.push_back(desc);
		}
	}
}

const std::vector<VSGE::MonoClassDesc*>& MonoApiDescriptions::GetClassesDescs() const {
	return _api_class_descs;
}