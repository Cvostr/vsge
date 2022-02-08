#include "TextEditWindow.hpp"
#include "TextEditWindows.hpp"
#include <Core/FileLoader.hpp>
#include <fstream>
#include <MonoScripting/MonoScriptStorage.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <EditorLayers/EditorLayer.hpp>

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

	_win_title = "Edit - " + 
		file_path.substr(EditorLayer::Get()->GetProject().GetAssetsDirectory().size() + 1);
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

void TextEditWindow::SetDeclsCS() {
	_text_editor->SetLanguageDefinition(TextEditWindows::Get()->GetCsLangDefinition());
	//TextEditor::LanguageDefinition::CPlusPlus();
}

void TextEditWindow::Draw() {
	ImGui::Begin(_win_title.c_str(), &_closed, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
	
	ImVec2 cur_size = ImGui::GetWindowSize();
	if (cur_size.x < 400 || cur_size.y < 300)
		ImGui::SetWindowSize(ImVec2(400, 300));

	DrawMenu();

	cur_pos = ImGui::GetCursorPos() + ImGui::GetWindowPos();

	_text_editor->Render("ed");

	if (_text_editor->IsCursorPositionChanged())
		_hint = "";

	if (_text_editor->IsTextChanged()) {
		TextEditor::Coordinates coord = _text_editor->GetCursorPosition();
		std::string str_line = _text_editor->GetCurrentLineText();
		for (auto c : str_line) {
			if (c == '\t')
				coord.mColumn -= _text_editor->GetTabSize() - 1;
		}
		str_line = str_line.substr(0, coord.mColumn);
		uint32 divider = 0;
		for (uint32 i = 0; i < str_line.size(); i ++) {
			char c = str_line[i];
			if (c == ' ' || c == ',' || c == '\t')
				divider = i;
		}
		
		if (!str_line.empty()) {
			str_line = str_line.substr(divider + 1);
			_hint = str_line;
		}
	}
	
	TextEditor::Coordinates coord = _text_editor->GetCursorPosition();
	cur_pos += ImVec2(coord.mColumn * 8, coord.mLine * 15);

	ImGui::End();

	DrawHints();
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

void TextEditWindow::DrawHints() {
	if (_hint.size() == 0)
		return;

	std::vector<const char*> hints;
	auto classes = TextEditWindows::Get()->GetClassesDescs();
	for (auto cls : classes) {
		if (cls->GetName().rfind(_hint, 0) == 0) {
			hints.push_back((cls->GetName()).c_str());
		}
	}

	if (hints.size() == 0)
		return;

	ImGui::Begin("hint", nullptr,
		ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_AlwaysAutoResize
		);

	ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
	
	ImGui::SetWindowPos(cur_pos);

	for (int i = 0; i < hints.size(); i++)
		if (ImGui::Selectable(hints[i])) {
			std::string to_insert = std::string(hints[i]).substr(_hint.size());
			_text_editor->InsertText(to_insert);
			_hint = "";
		}

	ImGui::End();
}