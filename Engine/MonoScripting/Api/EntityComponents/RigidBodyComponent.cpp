#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/RigidBodyComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static float GetMass(RigidBodyComponent* rgb) {
	return rgb->GetMass();
}
static void SetMass(RigidBodyComponent* rgb, float mass) {
	rgb->SetMass(mass);
}

static float GetFriction(RigidBodyComponent* rgb) {
	return rgb->GetFriction();
}
static void SetFriction(RigidBodyComponent* rgb, float mass) {
	rgb->SetFriction(mass);
}

static void ApplyCentralImpulse(RigidBodyComponent* rgb, Vec3 impulse) {
	rgb->ApplyCentralImpulse(impulse);
}
static void ApplyCentralForce(RigidBodyComponent* rgb, Vec3 force) {
	rgb->ApplyCentralForce(force);
}

void VSGE::BindRigidbody() {
	MonoScriptingLayer::AddInternalCall("RigidBodyComponent::i_GetMass(ulong)", GetMass);
	MonoScriptingLayer::AddInternalCall("RigidBodyComponent::i_SetMass(ulong,float)", SetMass);

	MonoScriptingLayer::AddInternalCall("RigidBodyComponent::i_GetFriction(ulong)", GetFriction);
	MonoScriptingLayer::AddInternalCall("RigidBodyComponent::i_SetFriction(ulong,float)", SetFriction);

	MonoScriptingLayer::AddInternalCall("RigidBodyComponent::i_ApplyCentralImpulse(ulong,Vec3)", ApplyCentralImpulse);
	MonoScriptingLayer::AddInternalCall("RigidBodyComponent::i_ApplyCentralForce(ulong,Vec3)", ApplyCentralForce);
}