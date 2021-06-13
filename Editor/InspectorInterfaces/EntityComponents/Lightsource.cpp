#include "EntityComponents.hpp"
#include <imgui.h>

using namespace VSGEditor;
using namespace VSGE;

void VSGEditor::DrawLightsourceComponent(VSGE::LightsourceComponent* light) {
	
	std::string shapes_str[3] = {
		"Directional Light",
		"Point Light",
		"Spot Light"
	};

	std::string current_shape = shapes_str[light->GetLightType()];

	if (ImGui::BeginCombo("Type", current_shape.c_str())) {
		for (uint32 i = 0; i < 3; i++) {
			bool is_selected = current_shape == shapes_str[i];
			if (ImGui::Selectable(shapes_str[i].c_str(), is_selected)) {
				current_shape = shapes_str[i];
				light->GetLightType() = (VSGE::LightType)(i);
			}
		}

		ImGui::EndCombo();
	}

	ImGui::ColorEdit3("Light color", &light->GetColor().r);

	ImGui::InputFloat("Intensity", &light->GetIntensity());
	ImGui::InputFloat("Range", &light->GetRange());
	if (light->GetLightType() == LIGHT_TYPE_SPOT) {
		ImGui::InputFloat("Spot Angle", &light->GetSpotAngle());
	}
}