#include "EditorSettingsWindow.hpp"

using namespace VSGEditor;

void EditorSettingsWindow::OnDrawWindow() {
	if (Draw("Settings")) {

        {
            ImGui::BeginChild("Child1", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 300), false);
            
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

        // Child 2: rounded border
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild("Child2", ImVec2(0, 300), true);
            
            ImGui::EndChild();
            ImGui::PopStyleVar();
        }

		ImGui::End();
	}
}