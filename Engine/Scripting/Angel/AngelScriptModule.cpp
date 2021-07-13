#include "AngelScriptModule.hpp"
#include "AngelScriptLayer.hpp"

using namespace VSGE;

AngelScriptModule::AngelScriptModule(asIScriptEngine* engine, const std::string& name) {
	_compileFailed = false;

	_streamWriter = new BytecodeStream;
	//Create module
	CreateModule(engine, name);
	//Set builder #include callback
	_builder.SetIncludeCallback(onInclude, this);
}

AngelScriptModule::AngelScriptModule() :
	_compileFailed(false),
	_streamWriter(new BytecodeStream),
	_module(nullptr)
{

}

AngelScriptModule::~AngelScriptModule() {
	if (_module) {
		_module->Discard();
	}

	SAFE_RELEASE(_streamWriter);
}

void AngelScriptModule::CreateModule(asIScriptEngine* engine, const std::string& name) {
	if (_module) {
		_module->Discard();
		_module = nullptr;
	}

	_builder.StartNewModule(engine, name.c_str());
	_module = engine->GetModule(name.c_str(), asGM_ONLY_IF_EXISTS);
}

void AngelScriptModule::Compile() {
	int result = _builder.BuildModule();

	if (result < 0) {
		//compile error occured
		_compileFailed = true;
	}
	else
		UpdateClassInfos();

}
bool AngelScriptModule::IsCompileFailed() {
	return _compileFailed;
}
void AngelScriptModule::SaveByteCode(const std::string dst_file) {
	_streamWriter->open_for_write(dst_file);
	_module->SaveByteCode(this->_streamWriter, true);
	_streamWriter->close();
}
void AngelScriptModule::LoadByteCode(byte* bytecode, uint32 size) {
	_streamWriter->open_for_read(bytecode, size);

	bool debugInfo = false;
	_module->LoadByteCode(this->_streamWriter, &debugInfo);
}
void AngelScriptModule::AddScript(byte* data, uint32 size, std::string label) {
	_builder.AddSectionFromMemory(label.c_str(), (const char*)data, size);
}

void AngelScriptModule::UpdateClassInfos() {
	_classInfos.clear();

	uint32 tc = _module->GetObjectTypeCount();
	for (uint32 n = 0; n < tc; n++)
	{
		bool found = false;
		asITypeInfo* type = _module->GetObjectTypeByIndex(n);
		uint32 ic = type->GetInterfaceCount();
		for (uint32 i = 0; i < ic; i++)
		{
			if (strcmp(type->GetInterface(i)->GetName(), "ZPScript") == 0)
			{
				MainClassDesc* desc = new MainClassDesc;
				desc->_name = type->GetName();
				desc->_info = type;
				//Get class fields amount
				desc->_classFieldsNum = type->GetPropertyCount();
				for (uint32 field_i = 0; field_i < desc->_classFieldsNum; field_i++) {
					ClassFieldDesc cf_desc;
					const char* _Name;
					type->GetProperty(field_i, &_Name, 
						&cf_desc.typeID, 
						&cf_desc.isPrivate, 
						&cf_desc.isProtected,
						0,
						&cf_desc.isReference);
					cf_desc.name = std::string(_Name);
					cf_desc.index = field_i;
					desc->_fields.push_back(cf_desc);
				}

				_classInfos.push_back(desc);
			}
		}
	}
	for (uint32 i = 0; i < _classInfos.size(); i++) {
		//asITypeInfo* __ClassInfo;
		//CreateClass(ZPSClassInfos[i]->Name, new GameObject, &ZPSClassInfos[i]->__SampleObject, &__ClassInfo);
	}
}

MainClassDesc* AngelScriptModule::GetMainClassDescByName(const std::string& className) {
	for (auto class_info : _classInfos) {
		if (class_info->_name == className)
			return class_info;
	}
	return nullptr;
}

int AngelScriptModule::CreateMainClassObject(const std::string& className, Entity* entity, asIScriptObject** ClassObj) {

	MainClassDesc* mainClassDesc = GetMainClassDescByName(className);
	AngelScriptLayer* layer = AngelScriptLayer::Get();

	return CreateMainClassObject(mainClassDesc, entity, ClassObj);
}

int AngelScriptModule::CreateMainClassObject(MainClassDesc* desc, Entity* entity, asIScriptObject** ClassObj) {
	int result = 0;
	AngelScriptLayer* layer = AngelScriptLayer::Get();

	std::string construct_str = desc->_name + " @" + desc->_name + "(GameObject@)";
	asIScriptFunction* factory = (desc->_info)->GetFactoryByDecl(construct_str.c_str());
	//Allocate class by Constructor
	{
		layer->SetContextFunction(factory);
		if (entity != nullptr)
			layer->ContextSetArgumentObject(0, entity);
		result = layer->ContextExecute();
	}
	//Get returned created class
	*ClassObj = *(asIScriptObject**)layer->GetAddressOfReturnValue();
	(*ClassObj)->AddRef();

	return result;
}

asIScriptFunction* MainClassDesc::GetFuncOnMainClass(const std::string& decl) {
	return _info->GetMethodByDecl(decl.c_str());
}
ClassFieldDesc* MainClassDesc::GetSuitableDesc(const std::string& name, int typeID, uint32 index) {
	for (unsigned int i = 0; i < this->_fields.size(); i++) {
		ClassFieldDesc* desc = &_fields[i];

		if (desc->index == index && desc->typeID == typeID && desc->name.compare(name) == 0)
			return desc;
		if (desc->typeID == typeID && desc->name.compare(name) == 0)
			return desc;
	}
	return nullptr;
}

int VSGE::onInclude(const char* include, const char* from, CScriptBuilder* builder, void* userParam) {
	//Engine::ScriptResource* DependencyRes = game_data->resources->getScriptByLabel(include);
	//include dependency
	//AGScriptMgr* mgr = static_cast<AGScriptMgr*>(userParam);
	//int result = mgr->AddScriptFile(DependencyRes);
	return 0;
}

void BytecodeStream::open_for_write(const std::string& output_file) {
	f = fopen(output_file.c_str(), "wb");
}
void BytecodeStream::close() {
	fclose(f); 
}
int BytecodeStream::Write(const void* ptr, asUINT size) {
	if (size == 0)
		return 0;
	fwrite(ptr, size, 1, f);
	return 1;
}

void BytecodeStream::open_for_read(byte* bytecode, uint32 size) {
	this->bytecode = bytecode;
	this->bytecode_size = size;
}

int BytecodeStream::Read(void* ptr, asUINT size){
	if (size == 0) 
		return 0;
	//fread(ptr, size, 1, f);
	return 1;
}