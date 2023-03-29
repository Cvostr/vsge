#include "ProjectSettingsWindow.hpp"
#include <InspectorInterfaces/ResourcePicker.hpp>
#include <imgui_stdlib.h>
#include <EditorLayers/EditorLayer.hpp>
#include <Engine/Application.hpp>
#include <fstream>
#include <Base/ImguiHelper.hpp>
#include <yaml-cpp/yaml.h>

using namespace VSGE;
using namespace VSGEditor;
using namespace YAML;

ProjectSettingsWindow::ProjectSettingsWindow() {
    _showCloseCross = true;
    _draw_mode = MODE_PROJECT;
    _main_scene.SetResourceType(VSGE::RESOURCE_TYPE_SCENE);
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
    DrawStringControl("Project name", _project_name);
    DrawResourcePicker("Start scene", _main_scene);
}
void ProjectSettingsWindow::DrawApplicationSettings() {
    DrawStringControl("Application name", _application_name);
    DrawIntControl("Version", _version);
    DrawStringControl("Version string", _version_string);
}

void ProjectSettingsWindow::SetDrawMode(DrawMode mode) {
    _draw_mode = mode;
}

void ProjectSettingsWindow::Save() {
    Application* app = Application::Get();
    EditorLayer* el = app->GetLayer<EditorLayer>();

    std::string project_cfg_path = el->GetProject().GetRootDirectory() + "/project.manifest";
    std::string app_cfg_path = el->GetProject().GetRootDirectory() + "/application.manifest";

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "project_name" << YAML::Value << _project_name;
    out << YAML::Key << "main_scene" << YAML::Value << _main_scene.GetResourceName();
    out << YAML::EndMap;
    std::ofstream fout(project_cfg_path);
    fout << out.c_str();
    fout.close();

    YAML::Emitter out_app;
    out_app << YAML::BeginMap;
    out_app << YAML::Key << "application_name" << YAML::Value << _application_name;
    out_app << YAML::Key << "version" << YAML::Value << _version;
    out_app << YAML::Key << "version_str" << YAML::Value << _version_string;
    out_app << YAML::EndMap;
    std::ofstream fout_app(app_cfg_path);
    fout_app << out_app.c_str();
    fout_app.close();
}

void ProjectSettingsWindow::LoadSettings() {
    Application* app = Application::Get();
    EditorLayer* el = app->GetLayer<EditorLayer>();
    std::string project_cfg_path = el->GetProject().GetRootDirectory() + "/project.manifest";
    std::string app_cfg_path = el->GetProject().GetRootDirectory() + "/application.manifest";

    Node data = YAML::LoadFile(project_cfg_path);
    _project_name = data["project_name"].as<std::string>();
    _main_scene.SetResource(data["main_scene"].as<std::string>());

    Node data_app = YAML::LoadFile(app_cfg_path);
    _application_name = data_app["application_name"].as<std::string>();
    _version = data_app["version"].as<int>();
    _version_string = data_app["version_str"].as<std::string>();
}