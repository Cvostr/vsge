#include "StartWindow.hpp"
#include "SceneWindow.hpp"
#include "InspectorWindow.hpp"
#include "SceneViewWindow.hpp"
#include "BrowserWindow.hpp"
#include <Engine/Application.hpp>
#include "../EditorLayers/EditorLayer.hpp"
#include <fstream>
#include <filesystem>
#include <Misc/DialogWindows.hpp>

using namespace VSGEditor;
using namespace YAML;
namespace fs = std::filesystem;

StartWindow::StartWindow() {
    ReadProjects();
}

bool StartWindow::ReadProjects() {
    Node data;
    try {
        data = YAML::LoadFile("projects.ini");
    }
    catch (YAML::BadFile) {
        return false;
    }


    if (!data["ProjectsList"])
        return false;

    Node projects = data["projects"];

    if (projects)
    {
        for (auto project : projects)
        {
            std::string proj_path = project.as<std::string>();
            _projects.push_back(proj_path);
        }
    }
    return true;
}

void StartWindow::SaveProjectsList() {
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "ProjectsList" << YAML::Value << "Untitled";
    out << YAML::Key << "projects" << YAML::Value << YAML::BeginSeq;

    for (auto& project : _projects) {
        out << Value << project;
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout("projects.ini");
    fout << out.c_str();
    fout.close();
}

void StartWindow::OnDrawWindow() {
    if (Draw("Start Window", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
        SetSize(600, 300);

        ImGui::Text("Projects");

        for (auto& str : _projects) {
            if (ImGui::Button(str.c_str())) {
                OpenProject(str);
            }
        }

        if (ImGui::Button("Create new project", ImVec2(_size.x / 2, 0))) {

        }
        ImGui::SameLine();
        if (ImGui::Button("Add existing project", ImVec2(_size.x / 2, 0))) {
            FileDialogDesc desc;
            desc.dialog_title = "Select project manifest of project";
            //desc.base_file_name = "project.manifest";
            desc.extensions = { {"VSGE manifest", "*.manifest"} };
            std::string path;
            OpenFileDialog(&desc, path);

            if (!path.empty()) {

                uint32 size = path.size() - 1;
                while (path[size] != '\\' && path[size] != '/') {
                    path.pop_back();
                    size--;
                }
                path.pop_back();
                //Add project root directory to list
                _projects.push_back(path);
                SaveProjectsList();
            }
        }

        ImGui::End();
    }
}

void StartWindow::OpenProject(const std::string& path) {
    if (!fs::is_directory(path)) {
        MessageDialogDesc desc;
        desc.dialog_title = "Error opening project";
        desc.message = "Directory " + path + " doesn't exist!";
        desc.buttons = MESSAGE_DIALOG_BTN_OK;
        desc.dialog_type = MESSAGE_DIALOG_TYPE_ERROR;
        DialogUserAction action;
        MessageDialog(&desc, action);
        if (action == DIALOG_USER_ACTION_ACCEPT) {
            return ;
        }
    }

    VSGE::Application* app = VSGE::Application::Get();
    if(!app->GetLayer<EditorLayer>()->OpenProject(Project(path, "assets", "cache")))
        Hide();
}