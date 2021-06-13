#include "EntityComponents.hpp"
#include <imgui.h>

void VSGEditor::DrawCameraComponent(VSGE::Camera* cac) {
	float near = cac->GetNearPlane();
	ImGui::InputFloat("Near plane", &near);
	cac->SetNearPlane(near);

	float far = cac->GetFarPlane();
	ImGui::InputFloat("Far plane", &far);
	cac->SetFarPlane(far);

	float fov = cac->GetFOV();
	ImGui::InputFloat("Field of view", &fov);
	cac->SetFOV(fov);
}