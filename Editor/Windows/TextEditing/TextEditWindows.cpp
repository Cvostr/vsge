#include "TextEditWindows.hpp"

using namespace VSGEditor;

TextEditWindows* TextEditWindows::_this = new TextEditWindows;

TextEditWindows::TextEditWindows() {
	_this = this;
}

TextEditWindows::~TextEditWindows() {
	for (auto it = text_edit_windows.begin(); it != text_edit_windows.end(); ++it) {
		delete (*it);
	}
	text_edit_windows.clear();
}

void TextEditWindows::AddWindow(TextEditWindow* window) {
	text_edit_windows.push_back(window);
}

void TextEditWindows::RemoveWindow(TextEditWindow* window) {
	text_edit_windows.remove(window);
}

TextEditWindow* TextEditWindows::OpenFile(const std::string& file_path) {
	//check, if file already opened
	for (auto it = text_edit_windows.begin(); it != text_edit_windows.end(); ++it) {
		if ((*it)->GetFilePath() == file_path)
			return nullptr;
	}
	//file isn't opened
	TextEditWindow* window = new TextEditWindow(file_path);
	AddWindow(window);
	return window;
}

void TextEditWindows::OnDrawWindow() {
	std::vector<TextEditWindow*> windows_to_remove;
	for (auto it = text_edit_windows.begin(); it != text_edit_windows.end(); ++it) {
		if ((*it)->IsClosed()) {
			windows_to_remove.push_back(*it);
		}
	}
	for (auto win : windows_to_remove) {
		RemoveWindow(win);
	}
	for (auto it = text_edit_windows.begin(); it != text_edit_windows.end(); ++it) {
		(*it)->Draw();
	}
}