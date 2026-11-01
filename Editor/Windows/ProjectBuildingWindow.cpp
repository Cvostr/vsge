#include "ProjectBuildingWindow.hpp"
#include <MonoScripting/MonoScriptStorage.hpp>
#include <EditorLayers/EditorLayer.hpp>
#include <Engine/Application.hpp>
#include <Base/ImguiHelper.hpp>

using namespace VSGE;
using namespace VSGEditor;

static const std::unordered_map<uint32, const char*> bundleMaxSizeSettings = {
       { 256,  "256 MiB" },
       { 512,  "512 MiB" },
       { 1024, "1 GiB" }
};

ProjectBuildingWindow::ProjectBuildingWindow() {
    _showCloseCross = true;
    _building = false;

    _packer = new ResourcePacker;
    _packer->SetBundleFileSplitSizeMegs(256);
}

ProjectBuildingWindow::~ProjectBuildingWindow() {
    delete _packer;
}

void ProjectBuildingWindow::OnDrawWindow() 
{
    if (Draw("Build project")) 
    {        
        if (_building)
            DrawBuildingInterface();
        else
            DrawPreBuildingInterface();

        ImGui::End();
    }
}

std::string ProjectBuildingWindow::GetDefaultOutputDirectory()
{
    Application* app = Application::Get();
    EditorLayer* el = app->GetLayer<EditorLayer>();

    Mpi::File outputDir(el->GetProject().GetRootDirectory() + "/built");
    return outputDir.getAbsolutePath();
}

void ProjectBuildingWindow::Build() 
{
    Mpi::File outputDirFile(outputDirPath);

    if (!outputDirFile.isDirectory()) {
        outputDirFile.mkdir();
    }

    _packer->SetOutputBundleDirectoryPath(outputDirPath);
    MonoScriptStorage::Get()->Compile();

    _packer->Run();
    _building = true;
}

void ProjectBuildingWindow::DrawPreBuildingInterface() 
{
    // reset base directory, if it is empty
    if (outputDirPath.empty())
        outputDirPath = GetDefaultOutputDirectory();

    DrawStringControl("Output Directory", outputDirPath);

    const char* currentMaxFileSizeStr = "Invalid";
    auto it = bundleMaxSizeSettings.find(_packer->GetBundleFileSplitSizeMegs());
    if (it != bundleMaxSizeSettings.end()) {
        currentMaxFileSizeStr = it->second;
    }

    if (DrawComboControl("Bundle file max size", currentMaxFileSizeStr))
    {
        for (const auto& [fileSize, label] : bundleMaxSizeSettings) 
        {
            bool is_selected = (_packer->GetBundleFileSplitSizeMegs() == fileSize);

            if (ImGui::Selectable(label, is_selected)) {
                _packer->SetBundleFileSplitSizeMegs(fileSize);
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::Button("Build", ImVec2(_size.x, 0))) {
        Build();
    }
}

void ProjectBuildingWindow::DrawBuildingInterface() 
{
    // Resource Packer finished its work
    bool finished = _packer->IsFinished() && _building;

    // Stop thread work, if resource packer finished its work
    if (finished && _packer->IsRunning()) {
        _packer->Stop();
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

    if (!finished)
    {
        // Draw progress bar
        float frac = ((float)_packer->GetCompleted()) / _packer->GetTotal();
        char message[120];
        std::snprintf(message, sizeof(message), "%zu / %zu", _packer->GetCompleted(), _packer->GetTotal());
        ImGui::ProgressBar(frac, ImVec2(-FLT_MIN, 0), message);
    } 
    else
    {
        // Draw OK button to switch interfaces
        if (ImGui::Button("OK", ImVec2(_size.x, 0))) {
            _building = false;
        }
    }
}