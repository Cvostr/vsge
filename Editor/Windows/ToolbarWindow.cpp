#include "ToolbarWindow.hpp"
#include "InspectorWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include <Scene/SceneLayer.hpp>
#include <Engine/Application.hpp>
#include <ImGuizmo.h>
#include <MonoScripting/MonoScriptStorage.hpp>

using namespace VSGEditor;

#define BTN_SIZE 35

void ToolbarWindow::OnDrawWindow() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::SceneLayer* scene_layer = app->GetLayer<VSGE::SceneLayer>();

    if (Draw("Toolbar" , ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
        )) {
        
        if (ImGui::Button("T", ImVec2(BTN_SIZE, BTN_SIZE))) {
            EditorLayer::Get()->SetTransformMode(ImGuizmo::OPERATION::TRANSLATE);
        }
        ImGui::SameLine();
        if (ImGui::Button("S", ImVec2(BTN_SIZE, BTN_SIZE))) {
            EditorLayer::Get()->SetTransformMode(ImGuizmo::OPERATION::SCALE);
        }
        ImGui::SameLine();
        if (ImGui::Button("R", ImVec2(BTN_SIZE, BTN_SIZE))) {
            EditorLayer::Get()->SetTransformMode(ImGuizmo::OPERATION::ROTATE);
        }

        ImGui::SameLine();
        if (!scene_layer->IsSceneRunning()) {
            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2, 0));
            if (ImGui::Button("Play", ImVec2(BTN_SIZE, BTN_SIZE))) {
                PlayScene();
            }
        }
        else {
            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2 - BTN_SIZE / 2, 0));
            if (ImGui::Button("Pause", ImVec2(BTN_SIZE, BTN_SIZE))) {
                PauseScene();
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop", ImVec2(BTN_SIZE, BTN_SIZE))) {
                StopScene();
            }
        }

        ImGui::End();
    }
    
    SetSize(app->GetWindow().GetWindowWidth(), 50);
}

void ToolbarWindow::PlayScene() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::SceneLayer* scene_layer = app->GetLayer<VSGE::SceneLayer>();

    if (!MonoScriptStorage::Get()->IsScriptingReady())
        return;

    EditorLayer::Get()->SetEditorCameraMode(EDITOR_CAMERA_MODE_GAME_CAMERA);

    if (!scene_layer->IsScenePaused()) {
        //Save scene
        scene_layer->BackupScene();
    }
    //Start scene execution
    scene_layer->StartScene();
}
void ToolbarWindow::PauseScene() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::SceneLayer* scene_layer = app->GetLayer<VSGE::SceneLayer>();

    scene_layer->PauseScene();
}
void ToolbarWindow::StopScene() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::SceneLayer* scene_layer = app->GetLayer<VSGE::SceneLayer>();

    EditorLayer::Get()->SetEditorCameraMode(EDITOR_CAMERA_MODE_EDIT_CAMERA);
    scene_layer->StopScene();

    InspectorWindow* insp = ImGuiLayer::Get()->GetWindow<InspectorWindow>();
    insp->SetShowingEntity(nullptr);
    EditorLayer::Get()->SetPickedEntity(nullptr);
    //Clear scene
    scene_layer->GetWorkingScene()->NewScene();
    //Deserialize temp scene
    scene_layer->RestoreScene();
}

ToolbarWindow::ToolbarWindow() {
    SetPos(0, 24);
}