#include "MonoScriptBlob.hpp"
#include "MonoScriptingLayer.hpp"
#include <Core/Logger.hpp>

using namespace VSGE;

MonoScriptBlob::MonoScriptBlob() :
	m_assembly(nullptr),
	m_image(nullptr)
{

}
MonoScriptBlob::~MonoScriptBlob() {
	ClearDescs();
}

void MonoScriptBlob::ClearDescs() {
	for (auto class_desc : m_class_descs) {
		delete class_desc;
	}
	m_class_descs.clear();
}

void MonoScriptBlob::Release() {
	if (m_assembly) {
		mono_assembly_close(m_assembly);
		//mono_free(_assembly);
		m_assembly = nullptr;
	}
	if (m_image) {
		//mono_free(_image);
		//_image = nullptr;
	}
}

MonoAssembly* MonoScriptBlob::GetAssembly() {
	return m_assembly;
}
MonoImage* MonoScriptBlob::GetImage() {
	return m_image;
}

MonoClassDesc* MonoScriptBlob::GetMonoClassDesc(const std::string& class_name, const std::string& namespace_name) {
	for (auto& desc : m_class_descs) {
		if (desc->GetName() == class_name)
			return desc;
	}
	return nullptr;
}

const std::vector<MonoClassDesc*>& MonoScriptBlob::GetClassesDescs() const {
	return m_class_descs;
}

int MonoScriptBlob::LoadFromFile(const std::string& path) 
{
	m_assembly = mono_domain_assembly_open(MonoScriptingLayer::Get()->GetDomain(), path.c_str());
	if (!m_assembly)
	{
		return 1;
	}

	m_image = mono_assembly_get_image(m_assembly);
	if (!m_image)
	{
		return 2;
	}

	BuildMonoClassDescsList();
	
	return 1;
}

MonoMethod* MonoScriptBlob::GetMethodByDescription(MonoMethodDesc* method_desc) {
	return mono_method_desc_search_in_image(method_desc, m_image);
}

void MonoScriptBlob::BuildMonoClassDescsList() {
	ClearDescs();

	const MonoTableInfo* table_info = mono_image_get_table_info(m_image, MONO_TABLE_TYPEDEF);
	//get count of c# classes
	int rows = mono_table_info_get_rows(table_info);

	for (int i = 0; i < rows; i++)
	{
		MonoClass* _class = nullptr;
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(m_image, cols[MONO_TYPEDEF_NAME]);
		const char* name_space = mono_metadata_string_heap(m_image, cols[MONO_TYPEDEF_NAMESPACE]);
		_class = mono_class_from_name(m_image, name_space, name);
		
		if(_class == nullptr) {
			Logger::Log(LogType::LOG_TYPE_ERROR) << "Error processing compiled Mono DLL. Class with name " << name << " is NULL\n";
			Logger::Log(LogType::LOG_TYPE_ERROR) << "Perhaps, api.dll is missing\n";
			continue;
		}

		MonoClassDesc* desc = new MonoClassDesc(_class);
		m_class_descs.push_back(desc);
	}
}