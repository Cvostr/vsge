#include "CreateProjectWindow.hpp"

using namespace VSGEditor;

void CreateProjectWindow::OnDrawWindow() {
    if (Draw("Create project")) {
        SetSize(600, 300);

        

        ImGui::End();
    }
}