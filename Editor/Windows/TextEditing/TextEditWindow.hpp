#pragma once

#include <string>
#include <vector>
#include <TextEditor.h>

namespace VSGEditor {
	class TextEditWindow {
	private:
		std::string _file_path;
		std::string _win_title;
		TextEditor* _text_editor;
		bool _closed;

		void DrawMenu();

		ImVec2 cur_pos;
		std::string _hint;
		void DrawHints();
	public:

		TextEditWindow(const std::string& file_path);

		~TextEditWindow();

		const std::string& GetFilePath() const;

		bool IsClosed() const;

		void SetDeclsCS();

		void Draw();
	};
}