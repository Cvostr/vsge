#include "CreateProjectWindow.hpp"
#include <imgui_stdlib.h>
#include <filesystem>
#include <mpi/Desktop/Dialog.hpp>
#include <fstream>
#include <Base/ProjectSettings.hpp>

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
                Mpi::MessageDialogDesc desc;
                desc.dialogTitle = "Error creating project";
                desc.dialogMessage = "Directory " + root_dir + " already exist!";
                desc.buttons = Mpi::MESSAGE_DIALOG_BTN_OK;
                desc.dialogType = Mpi::MESSAGE_DIALOG_TYPE_ERROR; 
                Mpi::DialogUserAction action = Mpi::Dialog::MessageDialog(desc);
                if (action == Mpi::DIALOG_USER_ACTION_ACCEPT) {
                    return;
                }
            }

            std::filesystem::create_directory(root_dir);
            std::filesystem::create_directory(root_dir + "/assets");
            std::filesystem::create_directory(root_dir + "/cache");
            std::filesystem::create_directory(root_dir + "/built");

            //Write settings files
            ProjectSettings settings;
            settings.setProjectName(project_name);
            settings.setApplicationName(project_name);
            settings.setVersion(0);
            settings.setVersionString("v0.0");
            settings.setStartupSceneId(Guid(0, 0, 0, 0));
            std::string json;
            settings.saveToJSON(json);

            std::ofstream fout(root_dir + "/config.json", std::ios::binary);
            fout.write(json.c_str(), json.size());
            fout.close();

            Hide();
        }

        ImGui::End();
    }
}