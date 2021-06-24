#include "EntityComponents.hpp"
#include <imgui.h>

void VSGEditor::DrawRigidbodyComponent(VSGE::RigidBodyComponent* rgbc) {
	float mass = rgbc->GetMass();
	ImGui::InputFloat("Mass", &mass);
	rgbc->SetMass(mass);

	float friction = rgbc->GetFriction();
	ImGui::InputFloat("Friction", &friction);
	rgbc->SetFriction(friction);
}