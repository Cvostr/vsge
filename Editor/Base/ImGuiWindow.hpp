#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <vector>
#include <imgui.h>
#include <imgui_internal.h>

namespace VSGEditor {
	class EditorWindow {
	protected:
		ImVec2 mSize;
		ImVec2 mPos;
		bool mInFocus;
	public:

		void UpdateWindowData() {
			mPos = ImGui::GetWindowPos();
			mSize = ImGui::GetWindowSize();

			mInFocus = ImGui::IsWindowFocused();
		}

		virtual void OnDrawWindow() = 0;
	};

	typedef std::vector<EditorWindow*> tEditorWindowList;
}