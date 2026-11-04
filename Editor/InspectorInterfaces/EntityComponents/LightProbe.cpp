#include "EntityComponents.hpp"
#include <Base/ImguiHelper.hpp>
#include <imgui.h>

void VSGEditor::DrawLightProbeComponent(VSGE::LightProbeComponent* ccc)
{
	float near = ccc->GetNearPlane();
	DrawFloatControl("Near plane", near);
	ccc->SetNearPlane(near);

	float far = ccc->GetFarPlane();
	DrawFloatControl("Far plane", far);
	ccc->SetFarPlane(far);

	float rad = ccc->GetRadius();
	DrawFloatControl("Radius", rad);
	ccc->SetRadius(rad);

	std::string bakedStatus = (ccc->IsBaked() ? "Yes" : "No");
	std::string bakedText = "Baked - " + bakedStatus;
	ImGui::Text(bakedText.c_str());

	if (ImGui::Button("Bake", ImVec2(ImGui::GetWindowWidth(), 0))) {
		ccc->QueueRender();
	}
}