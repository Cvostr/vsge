#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/CharacterControllerComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

bool IsOnGround(CharacterControllerComponent* ptr) {
	return ptr->IsOnGround();
}

void Move(CharacterControllerComponent* ptr, Vec2 direction) {
	ptr->Move(direction);
}

void VSGE::BindCharacterController() {
	MonoScriptingLayer::AddInternalCall("CharacterControllerComponent::i_IsOnGround(ulong)", IsOnGround);
	MonoScriptingLayer::AddInternalCall("CharacterControllerComponent::i_Move(ulong,Vec2)", Move);
}