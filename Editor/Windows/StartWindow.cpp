#include "StartWindow.hpp"
#include "CreateProjectWindow.hpp"
#include <Engine/Application.hpp>
#include <EditorLayers/EditorLayer.hpp>
#include <EditorLayers/ImGuiLayer.hpp>
#include <fstream>
#include <filesystem>
#include <mpi/Parse/Json/JsonReader.hpp>
#include <mpi/Parse/Json/JsonWriter.hpp>
#include <mpi/Desktop/Dialog.hpp>

using namespace VSGEditor;
using namespace Mpi;
namespace fs = std::filesystem;

#define PROJECTS_LIST_FILE "projects.json"

StartWindow::StartWindow() {
    ReadProjects();
}

bool StartWindow::ReadProjects()
{
    Mpi::File file(PROJECTS_LIST_FILE);
    if (!file.isFile())
    {
        return false;
    }

    std::ifstream stream = file.getIfstream(std::ios::binary);
    char* data = new char[file.getFileSize()];
    stream.read(data, file.getFileSize());
    stream.close();

    std::string jsonString = std::string(data);

    delete[] data;

    JsonReader reader(jsonString);
    JsonNode node = reader.parse();

    const JsonNode& projectsNode = node["projects"];
    for (int i = 0; i < projectsNode.size(); i++)
    {
        const JsonNode& projectNode = projectsNode[i];
        _projects.push_back(projectNode["path"].getValue<std::string>());
    }

    return true;
}

void StartWindow::SaveProjectsList() 
{
    JsonValue projectsArrayNode = Mpi::JsonValue::array();
    for (auto& project : _projects) {
        JsonValue projectNode = Mpi::JsonValue::object();
        projectNode.add("path", JsonValue(project));
        projectsArrayNode.add(projectNode);
    }

    Mpi::JsonNode cfgRootNode;
    cfgRootNode.add("projects", projectsArrayNode);

    std::string jsonSerialized;
    Mpi::JsonWriter writer(cfgRootNode);
    writer.write(jsonSerialized);

    std::ofstream fout(PROJECTS_LIST_FILE, std::ios::binary);
    fout.write(jsonSerialized.c_str(), jsonSerialized.size());
    fout.close();
}

void StartWindow::OnDrawWindow() 
{
    if (Draw("Start Window", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) 
    {
        SetSize(600, 300);

        ImGui::Text("Projects");

        for (auto& projectPath : _projects) 
        {
            if (ImGui::Button(projectPath.c_str())) 
            {
                try {
                    OpenProject(projectPath);
                }
                catch (const std::exception& ex) 
                {
                    Mpi::MessageDialogDesc desc;
                    desc.dialogTitle = "Error opening project";
                    desc.dialogMessage = ex.what();
                    desc.buttons = Mpi::MESSAGE_DIALOG_BTN_OK;
                    desc.dialogType = Mpi::MESSAGE_DIALOG_TYPE_ERROR;
                    Mpi::DialogUserAction action;
                    action = Mpi::Dialog::MessageDialog(desc);
                }
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove")) {
                    _projects.erase(std::remove(_projects.begin(), _projects.end(), projectPath), _projects.end());
                    SaveProjectsList();
                }

                ImGui::EndPopup();
            }
        }

        if (ImGui::Button("Create new project", ImVec2(_size.x / 2, 0))) {
            ImGuiLayer::Get()->GetWindow<CreateProjectWindow>()->Show();
        }

        ImGui::SameLine();
        if (ImGui::Button("Add existing project", ImVec2(_size.x / 2, 0))) {
            Mpi::FileDialogDesc desc;
            desc.dialogTitle = "Select project directory";
            std::string path = Mpi::Dialog::BrowseDirectoryDialog(desc);

            if (!path.empty()) 
            {
                // check is project already exists
                auto existent = std::find(_projects.begin(), _projects.end(), path);
                if (existent != _projects.end()) {
                    Mpi::MessageDialogDesc desc;
                    desc.dialogTitle = "Error adding project";
                    desc.dialogMessage = "Project already exists in list";
                    desc.buttons = Mpi::MESSAGE_DIALOG_BTN_OK;
                    desc.dialogType = Mpi::MESSAGE_DIALOG_TYPE_ERROR;
                    Mpi::Dialog::MessageDialog(desc);
                }
                else
                {
                    //Add project root directory to list
                    _projects.push_back(path);
                    SaveProjectsList();
                }
            }
        }

        ImGui::End();
    }
}

void StartWindow::OpenProject(const std::string& path) 
{
    if (!fs::is_directory(path)) 
    {
        throw std::runtime_error("Directory " + path + " doesn't exist!");
    }

    VSGE::Application* app = VSGE::Application::Get();
    if(!app->GetLayer<EditorLayer>()->OpenProject(Project(path, "assets", "cache")))
        Hide();
}