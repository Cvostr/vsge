#include "ToolbarWindow.hpp"
#include "InspectorWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include <Scene/SceneLayer.hpp>
#include <Engine/Application.hpp>
#include <ImGuizmo.h>
#include <MonoScripting/MonoScriptStorage.hpp>
#include <Misc/EditorIcons.hpp>
#include <mpi/Desktop/Dialog.hpp>
#include <exception>

using namespace VSGEditor;

#define BTN_SIZE 30

void ToolbarWindow::OnDrawWindow() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::SceneLayer* scene_layer = app->GetLayer<VSGE::SceneLayer>();

    if (Draw("Toolbar" , ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
        )) {
        
        if (ImGui::ImageButton(EditorIcons::Get()->GetTranslateIcon(), ImVec2(BTN_SIZE, BTN_SIZE))) {
            EditorLayer::Get()->SetTransformMode(ImGuizmo::OPERATION::TRANSLATE);
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(EditorIcons::Get()->GetScaleIcon(), ImVec2(BTN_SIZE, BTN_SIZE))) {
            EditorLayer::Get()->SetTransformMode(ImGuizmo::OPERATION::SCALE);
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(EditorIcons::Get()->GetRotateIcon(), ImVec2(BTN_SIZE, BTN_SIZE))) {
            EditorLayer::Get()->SetTransformMode(ImGuizmo::OPERATION::ROTATE);
        }

        ImGui::SameLine();
        if (!scene_layer->IsSceneRunning()) {
            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2, ImGui::GetCursorPosY()));
            if (ImGui::ImageButton(EditorIcons::Get()->GetPlayIcon(), ImVec2(BTN_SIZE, BTN_SIZE))) {
                PlayScene();
            }
        }
        else {
            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2 - BTN_SIZE / 2, ImGui::GetCursorPosY()));
            if (ImGui::ImageButton(EditorIcons::Get()->GetPauseIcon(), ImVec2(BTN_SIZE, BTN_SIZE))) {
                PauseScene();
            }
            ImGui::SameLine();
            if (ImGui::ImageButton(EditorIcons::Get()->GetStopIcon(), ImVec2(BTN_SIZE, BTN_SIZE))) {
                StopScene();
            }
        }

        ImGui::End();
    }
    
    SetSize(app->GetWindow().GetWindowWidth(), TOOLBAR_WINDOW_HEIGHT);
}

void ToolbarWindow::PlayScene() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::SceneLayer* scene_layer = app->GetLayer<VSGE::SceneLayer>();

    if (!MonoScriptStorage::Get()->IsScriptingReady())
        return;

    EditorLayer::Get()->SetEditorCameraMode(EDITOR_CAMERA_MODE_GAME_CAMERA);

    try {
        if (!scene_layer->IsScenePaused()) {
            //Save scene
            scene_layer->BackupScene();
        }
        //Start scene execution
        scene_layer->StartScene();
    }
    catch (const std::exception& ex) {
        StopScene();
        Mpi::MessageDialogDesc desc;
        desc.dialogTitle = "Error starting scene!";
        desc.dialogMessage = "Scene cannot be started.\nReason: " + std::string(ex.what());
        desc.dialogTitle = Mpi::MESSAGE_DIALOG_TYPE_ERROR;
        Mpi::Dialog::MessageDialog(desc);
    }
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
    scene_layer->GetMainScene()->NewScene();
    //Deserialize temp scene
    scene_layer->RestoreScene();
}

ToolbarWindow::ToolbarWindow() {
    SetPos(0, 24);
}