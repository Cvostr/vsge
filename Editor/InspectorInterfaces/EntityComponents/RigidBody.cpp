#include "EntityComponents.hpp"
#include <imgui.h>

void VSGEditor::DrawRigidbodyComponent(VSGE::RigidBodyComponent* rgbc) {
	float mass = rgbc->GetMass();
	ImGui::InputFloat("Mass", &mass);
	rgbc->SetMass(mass);
}