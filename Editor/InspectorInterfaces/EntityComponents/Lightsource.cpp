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
	if (light->GetLightType() != LIGHT_TYPE_DIRECTIONAL)
		ImGui::InputFloat("Range", &light->GetRange());

	if (light->GetLightType() == LIGHT_TYPE_SPOT) {
		ImGui::InputFloat("Spot Angle", &light->GetSpotAngle());
	}

	bool castShadows = light->GetCastShadows();
	ImGui::Checkbox("Cast shadows", &castShadows);
	light->SetCastShadows(castShadows);

	if (castShadows) {

		float shadowStrength = light->GetShadowStrength();
		ImGui::InputFloat("Shadow strength", &shadowStrength);
		light->SetShadowStrength(shadowStrength);

		float shadowsBias = light->GetShadowsBias();
		ImGui::InputFloat("Shadows Bias", &shadowsBias);
		light->SetShadowsBias(shadowsBias);

		float shadowsPcf = light->GetShadowPCF();
		ImGui::InputFloat("Shadows PCF", &shadowsPcf);
		light->SetShadowPCF(shadowsPcf);
	}
}