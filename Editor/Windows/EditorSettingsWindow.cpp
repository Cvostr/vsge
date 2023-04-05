#include "EditorSettingsWindow.hpp"
#include "../EditorLayers/EditorSettingsLayer.hpp"
#include <imgui_stdlib.h>

using namespace VSGEditor;


void EditorSettingsWindow::OnDrawWindow() {
	if (Draw("Settings")) {

        {
            ImGui::BeginChild("left_settings", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 300), false);
            
            if (ImGui::Button("General", ImVec2(ImGui::GetWindowWidth(), 0))) {
                _mode = Settings_General;
            }
            if (ImGui::Button("Scene view", ImVec2(ImGui::GetWindowWidth(), 0))) {
                _mode = Settings_SceneView;
            }
            if (ImGui::Button("Building", ImVec2(ImGui::GetWindowWidth(), 0))) {
                _mode = Settings_Building;
            }
            if (ImGui::Button("Mono", ImVec2(ImGui::GetWindowWidth(), 0))) {
                _mode = Settings_Mono;
            }

            ImGui::EndChild();
        }

        ImGui::SameLine();

        {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild("right_settings", ImVec2(0, 300), true);
            
            switch (_mode) {
            case Settings_General:
                DrawGeneralSettings();
                break;
            case Settings_SceneView:
                DrawSceneView();
                break;
            case Settings_Building:
                DrawBuilding();
                break;
            case Settings_Mono:
                DrawMonoSettings();
                break;
            }

            ImGui::EndChild();
            ImGui::PopStyleVar();
        }

		ImGui::End();
	}
}

void EditorSettingsWindow::DrawGeneralSettings() {

}
void EditorSettingsWindow::DrawSceneView() {

}
void EditorSettingsWindow::DrawBuilding() {

}
void EditorSettingsWindow::DrawMonoSettings() {
    EditorSettingsLayer* es = EditorSettingsLayer::Get();
    
    //ImGui::InputText("Path to mono", &es->_mono_path);
}