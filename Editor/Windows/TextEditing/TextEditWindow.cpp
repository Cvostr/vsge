#include "TextEditWindow.hpp"
#include <Core/FileLoader.hpp>
#include <fstream>

using namespace VSGEditor;

TextEditWindow::TextEditWindow(const std::string& file_path) {
	_file_path = file_path;
	_text_editor = new TextEditor;
	_closed = true;
	//read file
	char* data = nullptr;
	uint32 size = 0;
	VSGE::LoadFile(file_path, &data, &size, true);
	std::string str_text(data);
	_text_editor->SetText(str_text);
}

TextEditWindow::~TextEditWindow() {
	delete _text_editor;
}

const std::string& TextEditWindow::GetFilePath() const {
	return _file_path;
}

bool TextEditWindow::IsClosed() const {
	return !_closed;
}

void TextEditWindow::Draw() {
	std::string title = "Edit - " + _file_path;
	ImGui::Begin(title.c_str(), &_closed, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);

	DrawMenu();

	_text_editor->Render("ed");

	ImGui::End();
}

void TextEditWindow::DrawMenu() {
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				std::string textToSave = _text_editor->GetText();
				std::ofstream stream(_file_path);
				stream.write(textToSave.c_str(), textToSave.size());
				stream.close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, _text_editor->CanUndo()))
				_text_editor->Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, _text_editor->CanRedo()))
				_text_editor->Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, _text_editor->HasSelection()))
				_text_editor->Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, _text_editor->HasSelection()))
				_text_editor->Cut();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
				_text_editor->Paste();

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}