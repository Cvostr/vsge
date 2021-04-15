#include "SceneViewWindow.hpp"

using namespace VSGEditor;

void SceneViewWindow::OnDrawWindow() {
    ImGui::Begin("World view", nullptr, ImGuiWindowFlags_NoCollapse);
    //update current window pos and size
    UpdateWindowData();

    ImVec2 c = ImGui::GetCursorPos();

    const ImRect bb(mPos + c, mPos + c + mSize);

    /*ImGui::GetWindowDrawList()->AddImage(
        (void*)mRenderer->GetShaderResourceView(), bb.Min,
        bb.Max, ImVec2(1, 0), ImVec2(0, 1));
        */

    ImGui::End();
}