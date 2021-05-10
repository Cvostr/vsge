#include "ImGuiWindow.hpp"

using namespace VSGEditor;

void EditorWindow::UpdateWindowData() {
	mPos = ImGui::GetWindowPos();
	mSize = ImGui::GetWindowSize();

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