#include "EntityScriptComponent.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Core/YamlHelper.hpp>

using namespace YAML;
using namespace VSGE;

EntityScriptComponent::EntityScriptComponent() {
	_script_instance = new MonoScriptInstance;
}

EntityScriptComponent::~EntityScriptComponent() {
	SAFE_RELEASE(_script_instance)
}

void EntityScriptComponent::SetClassName(const std::string& class_name) {
	if (_class_name != class_name) {
		_class_name = class_name;
		_fields.clear();

		MonoScriptBlob* blob = MonoScriptingLayer::Get()->GetScriptsBlob();
		MonoClassDesc* class_desc = blob->GetMonoClassDesc(class_name);

		if (class_desc == nullptr)
			return;

		for (auto& field_desc : class_desc->GetFields()) {
			if (field_desc.GetVisibility() == FIELD_PRIVATE)
				continue;
			MonoScriptField field(&field_desc);
			_fields.push_back(field);
		}
	}
}

const std::string& EntityScriptComponent::GetClassName() {
	return _class_name;
}

MonoScriptInstance* EntityScriptComponent::GetInstance() {
	return _script_instance;
}

std::vector<MonoScriptField>& EntityScriptComponent::GetFields() {
	return _fields;
}

void EntityScriptComponent::Init() {
	_script_instance->CreateClassByName(_class_name);
	_script_instance->CallDefaultConstructor();
	_script_instance->SetValuePtrToField("entity_ptr", &_entity);

	for (auto& field : _fields) {
		_script_instance->SetValuePtrToField(field.GetDesc()->GetName(), field.GetValue().GetValuePtr());
	}
}

void EntityScriptComponent::OnStart() {
	_script_instance->CallOnStart();
}

void EntityScriptComponent::OnStop() {
	_script_instance->Release();
}

void EntityScriptComponent::OnUpdate() {
	_script_instance->CallOnUpdate();
}

void EntityScriptComponent::OnTriggerStay(Entity* entity) {
	if(IsActive())
		_script_instance->CallOnTriggerStay(entity);
}

void EntityScriptComponent::OnTriggerEnter(Entity* entity) {
	if (IsActive())
		_script_instance->CallOnTriggerEnter(entity);
}

void EntityScriptComponent::OnTriggerExit(Entity* entity) {
	if (IsActive())
		_script_instance->CallOnTriggerExit(entity);
}

void EntityScriptComponent::Serialize(YAML::Emitter& e) {
	e << Key << "class" << Value << GetClassName();
	//write fields
	e << YAML::Key << "fields" << YAML::Value << YAML::BeginSeq;
	for (auto& field : _fields) {
		e << BeginMap;
		e << Key << "name" << Value << field.GetDesc()->GetName();
		e << Key << "type" << Value << (int)field.GetDesc()->GetValueType();
		//e << Key << "value" << Value << field.GetValue().;
		e << EndMap;
	}

	e << YAML::EndSeq;

}
void EntityScriptComponent::Deserialize(YAML::Node& entity) {
	SetClassName(entity["class"].as<std::string>());
}

void EntityScriptComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(GetClassName());
}
void EntityScriptComponent::Deserialize(ByteSolver& solver) {
	SetClassName(solver.ReadNextString());
}