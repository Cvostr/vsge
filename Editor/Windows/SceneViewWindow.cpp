#include "SceneViewWindow.hpp"

using namespace VSGEditor;

void SceneViewWindow::OnDrawWindow() {
    if (Draw("World view")) {

        ImVec2 c = ImGui::GetCursorPos();

        const ImRect bb(mPos + c, mPos + c + mSize);

        /*ImGui::GetWindowDrawList()->AddImage(
            (void*)mRenderer->GetShaderResourceView(), bb.Min,
            bb.Max, ImVec2(1, 0), ImVec2(0, 1));
            */

        ImGui::End();
    }
}