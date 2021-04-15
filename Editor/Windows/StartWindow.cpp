#include "StartWindow.hpp"

#include "SceneWindow.hpp"
#include "InspectorWindow.hpp"
#include "SceneViewWindow.hpp"
#include "BrowserWindow.hpp"

using namespace VSGEditor;

void StartWindow::OnDrawWindow() {
    ImGui::Begin("Start Window", nullptr, ImGuiWindowFlags_NoCollapse);
    UpdateWindowData();

    
    ImGui::End();
}