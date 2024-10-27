#include "EntityComponents.hpp"
#include <Base/ImguiHelper.hpp>
#include <imgui.h>

void VSGEditor::DrawLightProbeComponent(VSGE::LightProbeComponent* ccc)
{

	float rad = ccc->GetRadius();
	DrawFloatControl("Near plane", rad);
	ccc->SetRadius(rad);

	std::string bakedStatus = (ccc->IsBaked() ? "Yes" : "No");
	std::string bakedText = "Baked - " + bakedStatus;
	ImGui::Text(bakedText.c_str());

	if (ImGui::Button("Bake", ImVec2(ImGui::GetWindowWidth(), 0))) {
		ccc->Bake();
	}
}