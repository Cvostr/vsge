#include "ToolbarWindow.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include <Scene/SceneLayer.hpp>
#include <Engine/Application.hpp>
#include <ImGuizmo.h>

using namespace VSGEditor;

#define BTN_SIZE 35

void ToolbarWindow::OnDrawWindow() {
    VSGE::Application* app = VSGE::Application::Get();
    VSGE::SceneLayer* scene_layer = app->GetLayer<VSGE::SceneLayer>();

    if (Draw("Toolbar" , (ImGuiWindowFlags_)(ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
        ))) {
        
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
                scene_layer->StartScene();
            }
        }
        else {
            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2 - BTN_SIZE / 2, 0));
            if (ImGui::Button("Pause", ImVec2(BTN_SIZE, BTN_SIZE))) {
                scene_layer->StopScene();
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop", ImVec2(BTN_SIZE, BTN_SIZE))) {
                scene_layer->StopScene();
            }
        }

        ImGui::End();
    }

    
    SetSize(app->GetWindow().GetWindowWidth(), 50);
}

ToolbarWindow::ToolbarWindow() {
    SetPos(0, 24);
}