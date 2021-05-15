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

bool EditorWindow::Draw(const std::string& title, ImGuiWindowFlags_ flags) {
	if (!_hidden) {

		if (_queuedResize) {
			ImGui::SetNextWindowSize(_size);
			_queuedResize = false;
			OnWindowResize();
		}

		if (_queuedMove) {
			ImGui::SetNextWindowPos(_pos);
			_queuedMove = false;
		}

		bool frozen = ImGuiLayer::Get()->IsWindowsFrozen();
		ImGuiWindowFlags_ _flags = flags;
		if (frozen)
			_flags = (ImGuiWindowFlags_)(_flags | (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize));

		ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | _flags);

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