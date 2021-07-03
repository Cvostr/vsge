#include "MonoScriptComponent.hpp"

using namespace VSGE;

MonoScriptComponent::MonoScriptComponent() {
	_instance = new MonoScriptInstance;
}

void MonoScriptComponent::OnUpdate() {
	_instance->CallUpdate();
}
void MonoScriptComponent::OnStart() {
	_instance->CallConstructor();
	_instance->CallStart();
}
void MonoScriptComponent::OnDestroy() {

}
void MonoScriptComponent::OnPreRender() {

}

void MonoScriptComponent::OnActivate() {

}
void MonoScriptComponent::OnDeactivate() {

}

void MonoScriptComponent::Serialize(YAML::Emitter& e) {

}
void MonoScriptComponent::Deserialize(YAML::Node& entity) {

}