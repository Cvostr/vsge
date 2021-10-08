#include "CreateProjectWindow.hpp"
#include <imgui_stdlib.h>
#include <filesystem>

using namespace VSGEditor;

void CreateProjectWindow::OnDrawWindow() {
    if (Draw("Create project")) {
        SetSize(600, 300);

        ImGui::Text("Project Name");
        ImGui::SameLine();
        ImGui::InputText("##Project Name", &project_name);

        ImGui::Separator();

        ImGui::Text("Path");
        ImGui::SameLine();
        ImGui::InputText("##Path", &project_directory);
        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {

        }

        if (ImGui::Button("Create", ImVec2(_size.x, 0))) {
            std::string root_dir = project_directory + "/" + project_name;

            if (std::filesystem::is_directory(root_dir)) {
                return; //error
            }

            std::filesystem::create_directory(root_dir);
            std::filesystem::create_directory(root_dir + "/assets");
            std::filesystem::create_directory(root_dir + "/cache");
        }

        ImGui::End();
    }
}