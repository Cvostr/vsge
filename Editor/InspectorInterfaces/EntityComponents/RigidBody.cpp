#include "EntityComponents.hpp"
#include <Base/ImguiHelper.hpp>

void VSGEditor::DrawRigidbodyComponent(VSGE::RigidBodyComponent* rgbc) {
	float mass = rgbc->GetMass();
	DrawFloatControl("Mass", mass);
	rgbc->SetMass(mass);

	float friction = rgbc->GetFriction();
	DrawFloatControl("Friction", friction);
	rgbc->SetFriction(friction);
}