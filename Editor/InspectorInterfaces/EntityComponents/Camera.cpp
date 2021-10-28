#include "EntityComponents.hpp"
#include "../ViewMaskEdit.hpp"
#include <imgui.h>
#include <Resources/Resource.hpp>
#include "../ResourcePicker.hpp"

using namespace VSGE;

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

	ViewMask current_view_mask = cac->GetViewMask();
	DrawViewMaskEditor(current_view_mask);
	cac->SetViewMask(current_view_mask);

	DrawResourcePicker(std::string("Render Target"), cac->GetTargetReference(), true);
}