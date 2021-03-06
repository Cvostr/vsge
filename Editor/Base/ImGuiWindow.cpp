#include "ImGuiWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;

void EditorWindow::UpdateWindowData() {
	ImVec2 old_size = _size;

	_pos = ImGui::GetWindowPos();
	_size = ImGui::GetWindowSize();

	if (_size.x != old_size.x || _size.y != old_size.y) {
		OnWindowResize();
	}

	_inFocus = ImGui::IsWindowFocused();
}

bool EditorWindow::Draw(const std::string& title, ImGuiWindowFlags flags) {
	if (_showing) {

		if (_queuedResize) {
			ImGui::SetNextWindowSize(_size);
			_queuedResize = false;
			OnWindowResize();
		}

		if (_queuedMove) {
			ImGui::SetNextWindowPos(_pos);
			_queuedMove = false;
		}

		bool* _closeCross = (_showCloseCross) ? &_showing : nullptr;
		ImGui::Begin(title.c_str(), _closeCross, ImGuiWindowFlags_NoCollapse | flags);

		if (!_showing)
			ImGui::End();

		//update current window pos and size
		UpdateWindowData();
	}
	return _showing;
}

bool EditorWindow::IsHidden() {
	return !_showing;
}

void EditorWindow::Hide() {
	_showing = false;
}

void EditorWindow::Show() {
	_showing = true;
}

bool EditorWindow::IsInFocus() {	return _inFocus;
}

void EditorWindow::SetSize(uint32 width, uint32 height) {
	_queuedResize = true;
	_size.x = (float)width;
	_size.y = (float)height;
}

void EditorWindow::SetPos(uint32 x, uint32 y) {
	_queuedMove = true;
	_pos.x = (float)x;
	_pos.y = (float)y;
}

const ImVec2& EditorWindow::GetSize() {
	return _size;
}

const ImVec2& EditorWindow::GetPos() {
	return _pos;
}

bool EditorWindow::isInsideWindow(uint32 x, uint32 y) {
	ImRect wrect = ImRect(_pos, _pos + _size);
	ImVec2 cursor_pos = ImVec2((float)x, (float)y);
	if (wrect.Min.x < cursor_pos.x && wrect.Min.y < cursor_pos.y && wrect.Max.x > x && wrect.Max.y > y)
		return true;
	return false;
}