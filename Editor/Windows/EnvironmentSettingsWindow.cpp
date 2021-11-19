#include "EnvironmentSettingsWindow.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include "../InspectorInterfaces/ResourcePicker.hpp"
#include <Scene/SceneLayer.hpp>

using namespace VSGEditor;
using namespace VSGE;

void EnvironmentSettingsWindow::OnDrawWindow(){
    if (Draw("Environment settings")) {
		
		Scene* scene = SceneLayer::Get()->GetMainScene();
		SceneEnvironmentSettings& settings = scene->GetEnvironmentSettings();

		Color ambient_color = settings.GetAmbientColor();
		ImGui::ColorEdit3("Light color", &ambient_color.r);
		settings.SetAmbientColor(ambient_color);

		DrawResourcePicker("Skybox material", settings._skybox_material);

		float shadows_distance = settings.GetMaxShadowDistance();
		ImGui::InputFloat("Shadow distance", &shadows_distance);
		settings.SetMaxShadowDistance(shadows_distance);

		int cascades_count = settings.GetShadowCascadesCount();
		ImGui::InputInt("Shadow cascades", &cascades_count);
		settings.SetShadowCascadesCount(cascades_count);

		settings.UpdateShadows();

		ImGui::End();
	}
}