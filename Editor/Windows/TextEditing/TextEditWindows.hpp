#pragma once

#include <list>
#include "../Base/ImGuiWindow.hpp"
#include "TextEditWindow.hpp"

namespace VSGEditor {
	class TextEditWindows : public EditorWindow {
	private:
		static TextEditWindows* _this;
		std::list<TextEditWindow*> text_edit_windows;
	public:

		TextEditWindows();

		~TextEditWindows();

		void AddWindow(TextEditWindow* window);

		void RemoveWindow(TextEditWindow* window);

		void OnDrawWindow();
	};
}