#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <string>
#include <vector>
#include <imgui.h>
#include <imgui_internal.h>

namespace VSGEditor {
	class EditorWindow {
	protected:
		ImVec2 mSize;
		ImVec2 mPos;
		bool _inFocus;
		bool _hidden;
	public:

		EditorWindow() :
			_inFocus(false),
			_hidden(false)
		{}

		void UpdateWindowData();

		bool Draw(const std::string& title, ImGuiWindowFlags_ flags = ImGuiWindowFlags_NoCollapse);

		bool IsHidden();

		void Hide();

		void Show();

		bool IsInFocus();

		virtual void OnDrawWindow() = 0;
	};

	typedef std::vector<EditorWindow*> tEditorWindowList;
}