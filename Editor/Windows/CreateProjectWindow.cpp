#include "CreateProjectWindow.hpp"
#include <imgui_stdlib.h>
#include <filesystem>
#include <Misc/DialogWindows.hpp>

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
                MessageDialogDesc desc;
                desc.dialog_title = "Error creating project";
                desc.message = "Directory " + root_dir + " already exist!";
                desc.buttons = MESSAGE_DIALOG_BTN_OK;
                desc.dialog_type = MESSAGE_DIALOG_TYPE_ERROR;
                DialogUserAction action;
                MessageDialog(&desc, action);
                if (action == DIALOG_USER_ACTION_ACCEPT) {
                    return;
                }
            }

            std::filesystem::create_directory(root_dir);
            std::filesystem::create_directory(root_dir + "/assets");
            std::filesystem::create_directory(root_dir + "/cache");
        }

        ImGui::End();
    }
}