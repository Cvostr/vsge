#pragma once

#include <string>
#include <TextEditor.h>

namespace VSGEditor {
	class TextEditWindow {
	private:
		std::string _file_path;
		std::string _win_title;
		TextEditor* _text_editor;
		bool _closed;

		void DrawMenu();
	public:

		TextEditWindow(const std::string& file_path);

		~TextEditWindow();

		const std::string& GetFilePath() const;

		bool IsClosed() const;

		void SetDeclsCS();

		void Draw();
	};
}