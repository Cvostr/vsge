#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <Core/VarTypes/Base.hpp>
#include <string>
#include <vector>
#include <imgui.h>
#include <imgui_internal.h>

#define WINDOW_MENU_OFFSET 26

namespace VSGEditor {
	class EditorWindow {
	protected:
		ImVec2 _size;
		ImVec2 _pos;
		bool _inFocus;
		bool _showing;
		bool _queuedResize;
		bool _queuedMove;
		bool _showCloseCross;
	public:

		EditorWindow() :
			_inFocus(false),
			_showing(true),
			_queuedResize(false),
			_queuedMove(false),
			_showCloseCross(false)
		{}

		void UpdateWindowData();

		bool Draw(const std::string& title, ImGuiWindowFlags_ flags = ImGuiWindowFlags_::ImGuiWindowFlags_None);

		bool IsHidden();

		void Hide();

		void Show();

		bool IsInFocus();

		bool isInsideWindow(uint32 x, uint32 y);

		void SetSize(uint32 width, uint32 height);

		void SetPos(uint32 x, uint32 y);

		const ImVec2& GetSize();

		const ImVec2& GetPos();

		virtual void OnDrawWindow() = 0;

		virtual void OnWindowResize(){}

		virtual void Regroup(uint32 width, uint32 height) {}
	};

	typedef std::vector<EditorWindow*> tEditorWindowList;
}