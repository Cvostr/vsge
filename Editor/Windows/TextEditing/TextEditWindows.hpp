#pragma once

#include <list>
#include <vector>
#include <Base/ImGuiWindow.hpp>
#include "TextEditWindow.hpp"
#include <MonoScripting/MonoClassDesc.hpp>

namespace VSGEditor {
	class TextEditWindows : public EditorWindow {
	private:
		static TextEditWindows* _this;
		std::list<TextEditWindow*> text_edit_windows;
		std::vector<VSGE::MonoClassDesc*> classes;
		TextEditor::LanguageDefinition* cs_lang;
	public:

		TextEditWindows();

		~TextEditWindows();

		static TextEditWindows* Get() {
			return _this;
		}

		void AddWindow(TextEditWindow* window);

		void RemoveWindow(TextEditWindow* window);

		TextEditWindow* OpenFile(const std::string& file_path);

		const TextEditor::LanguageDefinition& GetCsLangDefinition();

		std::vector<VSGE::MonoClassDesc*>& GetClassesDescs();

		void CreateLangDefinition();

		void OnDrawWindow();
	};
}