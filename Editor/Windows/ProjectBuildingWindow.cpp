#include "ProjectBuildingWindow.hpp"
#include <MonoScripting/MonoScriptStorage.hpp>
#include <EditorLayers/EditorLayer.hpp>
#include <Engine/Application.hpp>

using namespace VSGE;
using namespace VSGEditor;

ProjectBuildingWindow::ProjectBuildingWindow() {
    _showCloseCross = true;
    _building = false;

    _packer = new ResourcePacker;
}

ProjectBuildingWindow::~ProjectBuildingWindow() {
    delete _packer;
}

void ProjectBuildingWindow::OnDrawWindow() {
    if (Draw("Build project")) {
        
        if (_building)
            DrawBuildingInterface();
        else
            DrawPreBuildingInterface();

        ImGui::End();
    }
}

void ProjectBuildingWindow::Build() {
    Application* app = Application::Get();
    EditorLayer* el = app->GetLayer<EditorLayer>();

    Mpi::File outputDir(el->GetProject().GetRootDirectory() + "/built");

    if (!outputDir.isDirectory()) {
        outputDir.mkdir();
    }

    _packer->SetOutputBundleDirectoryPath(outputDir.getAbsolutePath());
    MonoScriptStorage::Get()->Compile();

    _packer->Run();
    _building = true;

}

void ProjectBuildingWindow::DrawPreBuildingInterface() 
{
    if (ImGui::Button("Build", ImVec2(_size.x, 0))) {
        Build();
    }
}

void ProjectBuildingWindow::DrawBuildingInterface() 
{
    if (_packer->IsFinished() && _building) {
        _packer->Stop();
        _building = false;
    }

    float progressBarHeight = ImGui::GetFrameHeight(); // Progress Bar height
    float spacing = ImGui::GetStyle().ItemSpacing.y;

    ImGui::BeginChild("LogRegion", ImVec2(0, -(progressBarHeight + spacing)), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextUnformatted(_packer->GetOutput().c_str());

    // Auto scroll to down
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    float frac = ((float)_packer->GetCompleted()) / _packer->GetTotal();
    char message[120];
    std::snprintf(message, sizeof(message), "%zu / %zu", _packer->GetCompleted(), _packer->GetTotal());
    ImGui::ProgressBar(frac, ImVec2(-FLT_MIN, 0), message);
}