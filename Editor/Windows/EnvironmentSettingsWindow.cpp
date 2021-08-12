#include "EnvironmentSettingsWindow.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include "../InspectorInterfaces/ResourcePicker.hpp"

using namespace VSGEditor;
using namespace VSGE;

void EnvironmentSettingsWindow::OnDrawWindow(){
    if (Draw("Environment settings")) {
		
		Scene* scene = EditorLayer::Get()->GetScene();

		ImGui::ColorEdit3("Light color", &scene->GetEnvironmentSettings()._ambient_color.r);

		DrawResourcePicker("Skybox material", scene->GetEnvironmentSettings()._skybox_material);

		float shadows_distance = scene->GetEnvironmentSettings().GetMaxShadowDistance();
		ImGui::InputFloat("Shadow distance", &shadows_distance);
		scene->GetEnvironmentSettings().SetMaxShadowDistance(shadows_distance);

		int cascades_count = scene->GetEnvironmentSettings().GetShadowCascadesCount();
		ImGui::InputInt("Shadow cascades", &cascades_count);
		scene->GetEnvironmentSettings().SetShadowCascadesCount(cascades_count);

		scene->GetEnvironmentSettings().UpdateShadows();

		ImGui::End();
	}
}