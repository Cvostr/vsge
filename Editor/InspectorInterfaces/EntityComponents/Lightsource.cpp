#include "EntityComponents.hpp"
#include <imgui.h>
#include <Base/ImguiHelper.hpp>

using namespace VSGEditor;
using namespace VSGE;

void VSGEditor::DrawLightsourceComponent(VSGE::LightsourceComponent* light) {
	
	std::string shapes_str[3] = {
		"Directional Light",
		"Point Light",
		"Spot Light"
	};

	std::string current_shape = shapes_str[light->GetLightType()];

	if (DrawComboControl("Type", current_shape)) {
		for (uint32 i = 0; i < 3; i++) {
			bool is_selected = current_shape == shapes_str[i];
			if (ImGui::Selectable(shapes_str[i].c_str(), is_selected)) {
				current_shape = shapes_str[i];
				light->GetLightType() = (VSGE::LightType)(i);
			}
		}

		ImGui::EndCombo();
	}

	DrawColorControl("Light color", light->GetColor());

	DrawFloatControl("Intensity", light->GetIntensity());
	if (light->GetLightType() != LIGHT_TYPE_DIRECTIONAL)
		DrawFloatControl("Range", light->GetRange());

	if (light->GetLightType() == LIGHT_TYPE_SPOT) {
		DrawFloatControl("Spot Angle", light->GetSpotAngle());
	}

	bool castShadows = light->GetCastShadows();
	DrawCheckboxControl("Cast shadows", castShadows);
	light->SetCastShadows(castShadows);

	if (castShadows) {

		float shadowStrength = light->GetShadowStrength();
		DrawFloatControl("Shadow strength", shadowStrength);
		light->SetShadowStrength(shadowStrength);

		float shadowsBias = light->GetShadowsBias();
		DrawFloatControl("Shadows Bias", shadowsBias);
		light->SetShadowsBias(shadowsBias);

		int32 shadowsPcf = light->GetShadowPCF();
		DrawIntControl("Shadows PCF", shadowsPcf);
		light->SetShadowPCF(shadowsPcf);
	}
}