#include "HoldOnWindow.hpp"
#include <Engine/Application.hpp>
#include <Engine/Window.hpp>

using namespace VSGEditor;

void HoldOnWindow::SetTaskDescription(const std::string& task) {
	_task_description = task;
}

void HoldOnWindow::OnDrawWindow() {
	if (Draw("Hold on...", ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse)) {

		ImGui::Text(_task_description.c_str());

		VSGE::Window& win = VSGE::Application::Get()->GetWindow();
		uint32 window_width = win.GetWindowWidth() / 2 - 150;
		uint32 window_height = win.GetWindowHeight() / 2 - 50;

		SetSize(300, 100);
		SetPos(window_width, window_height);

		ImGui::End();
	}
}