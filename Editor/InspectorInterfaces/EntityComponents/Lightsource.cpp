#include "EntityComponents.hpp"
#include <imgui.h>

using namespace VSGEditor;
using namespace VSGE;

void VSGEditor::DrawLightsourceComponent(VSGE::LightsourceComponent* light) {
	ImGui::RadioButton("Point Light", (int*)&light->GetLightType(), (int)LightType::LIGHT_TYPE_POINT);
	ImGui::RadioButton("Spot Light", (int*)&light->GetLightType(), (int)LightType::LIGHT_TYPE_SPOT);

	ImGui::ColorEdit3("Light color", &light->GetColor().r);

	ImGui::InputFloat("Intensity", &light->GetIntensity());
	ImGui::InputFloat("Range", &light->GetRange());
}