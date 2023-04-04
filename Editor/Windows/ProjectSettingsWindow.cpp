#include "ProjectSettingsWindow.hpp"
#include <InspectorInterfaces/ResourcePicker.hpp>
#include <imgui_stdlib.h>
#include <EditorLayers/EditorLayer.hpp>
#include <Engine/Application.hpp>
#include <fstream>
#include <Base/ImguiHelper.hpp>
#include <mpi/Parse/Json/JsonReader.hpp>
#include <mpi/Parse/Json/JsonWriter.hpp>

using namespace VSGE;
using namespace VSGEditor;

ProjectSettingsWindow::ProjectSettingsWindow() {
    _showCloseCross = true;
    _draw_mode = MODE_PROJECT;
}

void ProjectSettingsWindow::OnDrawWindow(){
    if (Draw("Project settings")) {
        SetSize(600, 300);

        if (_draw_mode == MODE_PROJECT)
            DrawProjectSettings();
        else if (_draw_mode == MODE_APP)
            DrawApplicationSettings();

        if (ImGui::Button("Save", ImVec2(_size.x, 0))) {
            Save();
        }

        ImGui::End();
    }
}

void ProjectSettingsWindow::DrawProjectSettings() {
    std::string projectName = EditorLayer::Get()->GetProject().getSettings().getProjectName();
    DrawStringControl("Project name", projectName);
    EditorLayer::Get()->GetProject().getSettings().setProjectName(projectName);

    ResourceReference& startupScene = EditorLayer::Get()->GetProject().getSettings().getStartupSceneResource();
    DrawResourcePicker("Start scene", startupScene, VSGE::RESOURCE_TYPE_SCENE);
}
void ProjectSettingsWindow::DrawApplicationSettings() {
    std::string applicationName = EditorLayer::Get()->GetProject().getSettings().getApplicationName();
    DrawStringControl("Application name", applicationName);
    EditorLayer::Get()->GetProject().getSettings().setApplicationName(applicationName);

    int version = EditorLayer::Get()->GetProject().getSettings().getVersion();
    DrawIntControl("Version", version);
    EditorLayer::Get()->GetProject().getSettings().setVersion(version);
    
    std::string versionString = EditorLayer::Get()->GetProject().getSettings().getVersionString();
    DrawStringControl("Version string", versionString);
    EditorLayer::Get()->GetProject().getSettings().setVersionString(versionString);
}

void ProjectSettingsWindow::SetDrawMode(DrawMode mode) {
    _draw_mode = mode;
}

void ProjectSettingsWindow::Save() {
    Application* app = Application::Get();
    EditorLayer* el = app->GetLayer<EditorLayer>();

    el->GetProject().saveSettings();
  
}