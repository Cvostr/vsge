#include "EntityScriptComponent.hpp"

using namespace VSGE;

EntityScriptComponent::EntityScriptComponent() {
	_script_instance = new MonoScriptInstance;
}

EntityScriptComponent::~EntityScriptComponent() {
	SAFE_RELEASE(_script_instance)
}

void EntityScriptComponent::SetClassName(const std::string& class_name) {
	_class_name = class_name;
}

const std::string& EntityScriptComponent::GetClassName() {
	return _class_name;
}

MonoScriptInstance* EntityScriptComponent::GetInstance() {
	return _script_instance;
}

void EntityScriptComponent::Init() {
	_script_instance->CreateClassByName(_class_name);
	_script_instance->CallDefaultConstructor();
	_script_instance->SetValuePtrToField("entity_ptr", &(void*)_entity);
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