#include "ProjectSettingsWindow.hpp"
#include <InspectorInterfaces/ResourcePicker.hpp>
#include <imgui_stdlib.h>
#include <EditorLayers/EditorLayer.hpp>
#include <Engine/Application.hpp>
#include <fstream>

using namespace VSGE;
using namespace VSGEditor;
using namespace YAML;

ProjectSettingsWindow::ProjectSettingsWindow() {
    _showCloseCross = true;
    _main_scene.SetResourceType(VSGE::RESOURCE_TYPE_SCENE);
}

void ProjectSettingsWindow::OnDrawWindow(){
    if (Draw("Project settings")) {
        SetSize(600, 300);

        ImGui::InputText("Project name", &_project_name);

        DrawResourcePicker("Start scene", _main_scene);

        if (ImGui::Button("Save", ImVec2(_size.x, 0))) {
            Save();
        }

        ImGui::End();
    }
}

void ProjectSettingsWindow::Save() {
    Application* app = Application::Get();
    EditorLayer* el = app->GetLayer<EditorLayer>();

    std::string project_cfg_path = el->GetProject().GetRootDirectory() + "/project.manifest";

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "project_name" << YAML::Value << _project_name;
    out << YAML::Key << "main_scene" << YAML::Value << _main_scene.GetResourceName();
    out << YAML::EndMap;
    std::ofstream fout(project_cfg_path);
    fout << out.c_str();
    fout.close();
}

void ProjectSettingsWindow::LoadSettings() {
    Application* app = Application::Get();
    EditorLayer* el = app->GetLayer<EditorLayer>();
    std::string project_cfg_path = el->GetProject().GetRootDirectory() + "/project.manifest";

    Node data = YAML::LoadFile(project_cfg_path);

    _project_name = data["project_name"].as<std::string>();
    _main_scene.SetResource(data["main_scene"].as<std::string>());
}