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
	mono_add_internal_call("IEntityComponent::i_SetActive(ulong,bool)", SetActive);
	mono_add_internal_call("IEntityComponent::i_IsActive(ulong)", IsActive);
	mono_add_internal_call("IEntityComponent::i_GetEntity(ulong)", GetEntity);
	mono_add_internal_call("IEntityComponent::i_GetType(ulong)", GetType);

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