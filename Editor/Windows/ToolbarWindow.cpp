#include "ToolbarWindow.hpp"
#include "../EditorLayers/EditorLayer.hpp"
#include <ImGuizmo.h>

using namespace VSGEditor;

#define BTN_SIZE 35

void ToolbarWindow::OnDrawWindow() {
    if (Draw("Toolbar_window", (ImGuiWindowFlags_)(ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse))) {
        
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

        ImGui::End();
    }
}

ToolbarWindow::ToolbarWindow() {
    SetSize(140, 50);
    SetPos(0, 20);
}