#include "EntityScriptComponent.hpp"

using namespace VSGE;

EntityScriptComponent::EntityScriptComponent() {
	_script_instance = new MonoScriptInstance;
}

EntityScriptComponent::~EntityScriptComponent() {
	SAFE_RELEASE(_script_instance)
}

void EntityScriptComponent::SetClassName(const std::string& class_name) {
	_script_instance->CreateClassByName(class_name);
}

const std::string& EntityScriptComponent::GetClassName() {
	return _script_instance->GetClassName();
}