#include "EntityScriptComponent.hpp"

using namespace VSGE;

void EntityScriptComponent::SetScriptName(const std::string& script) {
	_scriptClassName = script;

	instance->CreateInstance(script, GetEntity());
}

const std::string& EntityScriptComponent::GetScriptName() {
	return _scriptClassName;
}

void EntityScriptComponent::OnUpdate() {
	instance->CallOnUpdate();
}
void EntityScriptComponent::OnStart() {
	instance->CallOnStart();
}
void EntityScriptComponent::OnDestroy() {

}
void EntityScriptComponent::OnPreRender() {

}

void EntityScriptComponent::OnActivate() {

}
void EntityScriptComponent::OnDeactivate() {

}