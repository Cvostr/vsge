#include "StartWindow.hpp"
#include "SceneWindow.hpp"
#include "InspectorWindow.hpp"
#include "SceneViewWindow.hpp"
#include "BrowserWindow.hpp"
#include <Engine/Application.hpp>
#include "../EditorLayers/EditorLayer.hpp"
#include <fstream>

using namespace VSGEditor;
using namespace YAML;


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
}

void StartWindow::OnDrawWindow() {
    if (Draw("Start Window", ImGuiWindowFlags_NoMove)) {
        SetSize(600, 300);
        UpdateWindowData();

        for (auto& str : _projects) {
            if (ImGui::Button(str.c_str())) {
                OpenProject(str);
            }
        }

        if (ImGui::Button("Add Internal Test")) {
            _projects.push_back("D:\\DEV\\vsge_test_assets\\res");
            SaveProjectsList();
        }

        ImGui::End();
    }
}

void StartWindow::OpenProject(const std::string& path) {
    VSGE::Application* app = VSGE::Application::Get();
    app->GetLayer<EditorLayer>()->OpenProjectDirectory(path);
    Hide();
}