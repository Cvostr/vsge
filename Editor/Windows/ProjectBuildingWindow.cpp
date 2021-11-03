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

    _packer->SetOutputBundleDirectoryPath(el->GetProject().GetRootDirectory() + "/built");
    MonoScriptStorage::Get()->Compile();
    _packer->Run();
    _building = true;
}

void ProjectBuildingWindow::DrawPreBuildingInterface() {
    if (ImGui::Button("Build", ImVec2(_size.x, 0))) {
        Build();
    }
}

void ProjectBuildingWindow::DrawBuildingInterface() {
    if (_packer->IsFinished() && _building) {
        _packer->Stop();
        _building = false;
    }

    ImGui::Text(_packer->GetOutput().c_str());
}