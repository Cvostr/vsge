#include "ImGuiWindow.hpp"

using namespace VSGEditor;

void EditorWindow::UpdateWindowData() {
	_pos = ImGui::GetWindowPos();
	_size = ImGui::GetWindowSize();

	_inFocus = ImGui::IsWindowFocused();
}

bool EditorWindow::Draw(const std::string& title, ImGuiWindowFlags_ flags) {
	if (!_hidden) {
		ImGui::Begin(title.c_str(), nullptr, flags);
		//update current window pos and size
		UpdateWindowData();
	}
	return !_hidden;
}

bool EditorWindow::IsHidden() {
	return _hidden;
}

void EditorWindow::Hide() {
	_hidden = true;
}

void EditorWindow::Show() {
	_hidden = false;
}

bool EditorWindow::IsInFocus() {
	return _inFocus;
}

bool EditorWindow::isInsideWindow(uint32 x, uint32 y) {
	ImRect wrect = ImRect(_pos, _pos + _size);
	ImVec2 cursor_pos = ImVec2(x, y);
	if (wrect.Min.x < cursor_pos.x && wrect.Min.y < cursor_pos.y && wrect.Max.x > x && wrect.Max.y > y)
		return true;
	return false;
}