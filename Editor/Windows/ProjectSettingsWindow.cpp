#include "ProjectSettingsWindow.hpp"
#include <InspectorInterfaces/ResourcePicker.hpp>
#include <imgui_stdlib.h>

using namespace VSGEditor;

void ProjectSettingsWindow::OnDrawWindow(){
    if (Draw("Project settings")) {
        SetSize(600, 300);

        ImGui::InputText("Project name", &_project_name);

        DrawResourcePicker("Start scene", _main_scene);

        if (ImGui::Button("Save", ImVec2(_size.x, 0))) {

        }

        ImGui::End();
    }
}