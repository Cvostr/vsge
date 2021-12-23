#include "PlayerViewWindow.hpp"
#include <vulkan/vulkan.hpp>

using namespace VSGEditor;

PlayerViewWindow::PlayerViewWindow() {
	texture = VK_NULL_HANDLE;
}

void PlayerViewWindow::OnDrawWindow() {
    if (Draw("Player view", ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar)) {
        //ImGui::PopStyleColor();

        const ImRect bb(_pos, _pos + _size);

        ImGui::End();
    }
}

void PlayerViewWindow::OnWindowResize() {

}