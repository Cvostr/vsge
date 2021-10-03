#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/ColliderComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static Vec3 IsTrigger(ColliderComponent* cc) {
	return cc->IsTrigger();
}
static void SetTrigger(ColliderComponent* cc, bool trigger) {
	cc->SetTrigger(trigger);
}

static Vec3 GetSize(ColliderComponent* cc) {
	return cc->GetSize();
}
static void SetSize(ColliderComponent* cc, Vec3 size) {
	cc->SetSize(size);
}

static Vec3 GetCenter(ColliderComponent* cc) {
	return cc->GetCenter();
}
static void SetCenter(ColliderComponent* cc, Vec3 center) {
	cc->SetCenter(center);
}

void VSGE::BindCollider() {
	mono_add_internal_call("ColliderComponent::i_IsTrigger(ulong)", IsTrigger);
	mono_add_internal_call("RigidBodyComponent::i_SetTrigger(ulong,bool)", SetTrigger);

	mono_add_internal_call("ColliderComponent::i_GetSize(ulong)", GetSize);
	mono_add_internal_call("RigidBodyComponent::i_SetSize(ulong,Vec3)", SetSize);

	mono_add_internal_call("ColliderComponent::i_GetCenter(ulong)", GetCenter);
	mono_add_internal_call("RigidBodyComponent::i_SetCenter(ulong,Vec3)", SetCenter);
}