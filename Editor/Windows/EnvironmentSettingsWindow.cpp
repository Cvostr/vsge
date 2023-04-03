#include "EnvironmentSettingsWindow.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include "../InspectorInterfaces/ResourcePicker.hpp"
#include <Scene/SceneLayer.hpp>
#include <Base/ImguiHelper.hpp>

using namespace VSGEditor;
using namespace VSGE;

void EnvironmentSettingsWindow::OnDrawWindow(){
    if (Draw("Environment settings")) {
		
		Scene* scene = SceneLayer::Get()->GetMainScene();
		SceneEnvironmentSettings& settings = scene->GetEnvironmentSettings();

		Color ambient_color = settings.GetAmbientColor();
		DrawColorControl("Light color", ambient_color);
		settings.SetAmbientColor(ambient_color);

		DrawResourcePicker("Skybox material", settings._skybox_material, VSGE::RESOURCE_TYPE_MATERIAL);

		float shadows_distance = settings.GetMaxShadowDistance();
		DrawFloatControl("Shadow distance", shadows_distance);
		settings.SetMaxShadowDistance(shadows_distance);

		int cascades_count = settings.GetShadowCascadesCount();
		DrawIntControl("Shadow cascades", cascades_count);
		settings.SetShadowCascadesCount(cascades_count);

		settings.UpdateShadows();

		ImGui::End();
	}
}