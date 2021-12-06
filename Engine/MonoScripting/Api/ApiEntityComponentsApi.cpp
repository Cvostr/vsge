#include "ApiBindings.hpp"
#include <Scene/IEntityComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void SetActive(void* ptr, bool active) {
	((IEntityComponent*)ptr)->SetActive(active);
}

static bool IsActive(void* ptr) {
	return ((IEntityComponent*)ptr)->IsActive();
}

static void* GetEntity(void* ptr) {
	return ((IEntityComponent*)ptr)->GetEntity();
}

static EntityComponentType GetType(void* ptr) {
	return ((IEntityComponent*)ptr)->GetType();
}


void VSGE::BindEntityComponentsApi() {
	MonoScriptingLayer::AddInternalCall("IEntityComponent::i_SetActive(ulong,bool)", SetActive);
	MonoScriptingLayer::AddInternalCall("IEntityComponent::i_IsActive(ulong)", IsActive);
	MonoScriptingLayer::AddInternalCall("IEntityComponent::i_GetEntity(ulong)", GetEntity);
	MonoScriptingLayer::AddInternalCall("IEntityComponent::i_GetType(ulong)", GetType);

	BindLightsource();
	BindAudiosource();
	BindRigidbody();
	BindCollider();
	BindCamera();
	BindMaterial();
	BindMesh();
	BindCharacterController();
	BindParticleEmitter();
}