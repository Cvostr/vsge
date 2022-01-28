#include "TextEditWindows.hpp"

using namespace VSGEditor;

TextEditWindows* TextEditWindows::_this = new TextEditWindows;

TextEditWindows::TextEditWindows() {
	_this = this;
}

TextEditWindows::~TextEditWindows() {

}

void TextEditWindows::AddWindow(TextEditWindow* window) {
	text_edit_windows.push_back(window);
}

void TextEditWindows::RemoveWindow(TextEditWindow* window) {
	text_edit_windows.remove(window);
}

void TextEditWindows::OnDrawWindow() {
	for (auto it = text_edit_windows.begin(); it != text_edit_windows.end(); ++it) {
		(*it)->Draw();
	}
}