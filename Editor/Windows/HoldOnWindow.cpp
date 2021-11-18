#include "HoldOnWindow.hpp"

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

		SetSize(300, 100);

		ImGui::End();
	}
}