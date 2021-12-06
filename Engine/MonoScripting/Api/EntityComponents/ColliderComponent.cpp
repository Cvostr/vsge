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
	MonoScriptingLayer::AddInternalCall("ColliderComponent::i_IsTrigger(ulong)", IsTrigger);
	MonoScriptingLayer::AddInternalCall("ColliderComponent::i_SetTrigger(ulong,bool)", SetTrigger);

	MonoScriptingLayer::AddInternalCall("ColliderComponent::i_GetSize(ulong)", GetSize);
	MonoScriptingLayer::AddInternalCall("ColliderComponent::i_SetSize(ulong,Vec3)", SetSize);

	MonoScriptingLayer::AddInternalCall("ColliderComponent::i_GetCenter(ulong)", GetCenter);
	MonoScriptingLayer::AddInternalCall("ColliderComponent::i_SetCenter(ulong,Vec3)", SetCenter);
}