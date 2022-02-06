#include "TextEditWindow.hpp"
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


static bool TokenizeCStyleString(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
{
	const char* p = in_begin;

	if (*p == '"')
	{
		p++;

		while (p < in_end)
		{
			// handle end of string
			if (*p == '"')
			{
				out_begin = in_begin;
				out_end = p + 1;
				return true;
			}

			// handle escape character for "
			if (*p == '\\' && p + 1 < in_end && p[1] == '"')
				p++;

			p++;
		}
	}

	return false;
}

static bool TokenizeCStyleCharacterLiteral(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
{
	const char* p = in_begin;

	if (*p == '\'')
	{
		p++;

		// handle escape characters
		if (p < in_end && *p == '\\')
			p++;

		if (p < in_end)
			p++;

		// handle end of character literal
		if (p < in_end && *p == '\'')
		{
			out_begin = in_begin;
			out_end = p + 1;
			return true;
		}
	}

	return false;
}

static bool TokenizeCStyleIdentifier(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
{
	const char* p = in_begin;

	if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
	{
		p++;

		while ((p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_'))
			p++;

		out_begin = in_begin;
		out_end = p;
		return true;
	}

	return false;
}

static bool TokenizeCStyleNumber(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
{
	const char* p = in_begin;

	const bool startsWithNumber = *p >= '0' && *p <= '9';

	if (*p != '+' && *p != '-' && !startsWithNumber)
		return false;

	p++;

	bool hasNumber = startsWithNumber;

	while (p < in_end && (*p >= '0' && *p <= '9'))
	{
		hasNumber = true;

		p++;
	}

	if (hasNumber == false)
		return false;

	bool isFloat = false;
	bool isHex = false;
	bool isBinary = false;

	if (p < in_end)
	{
		if (*p == '.')
		{
			isFloat = true;

			p++;

			while (p < in_end && (*p >= '0' && *p <= '9'))
				p++;
		}
		else if (*p == 'x' || *p == 'X')
		{
			// hex formatted integer of the type 0xef80

			isHex = true;

			p++;

			while (p < in_end && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
				p++;
		}
		else if (*p == 'b' || *p == 'B')
		{
			// binary formatted integer of the type 0b01011101

			isBinary = true;

			p++;

			while (p < in_end && (*p >= '0' && *p <= '1'))
				p++;
		}
	}

	if (isHex == false && isBinary == false)
	{
		// floating point exponent
		if (p < in_end && (*p == 'e' || *p == 'E'))
		{
			isFloat = true;

			p++;

			if (p < in_end && (*p == '+' || *p == '-'))
				p++;

			bool hasDigits = false;

			while (p < in_end && (*p >= '0' && *p <= '9'))
			{
				hasDigits = true;

				p++;
			}

			if (hasDigits == false)
				return false;
		}

		// single precision floating point type
		if (p < in_end && *p == 'f')
			p++;
	}

	if (isFloat == false)
	{
		// integer size type
		while (p < in_end && (*p == 'u' || *p == 'U' || *p == 'l' || *p == 'L'))
			p++;
	}

	out_begin = in_begin;
	out_end = p;
	return true;
}

static bool TokenizeCStylePunctuation(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end)
{
	(void)in_end;

	switch (*in_begin)
	{
	case '[':
	case ']':
	case '{':
	case '}':
	case '!':
	case '%':
	case '^':
	case '&':
	case '*':
	case '(':
	case ')':
	case '-':
	case '+':
	case '=':
	case '~':
	case '|':
	case '<':
	case '>':
	case '?':
	case ':':
	case '/':
	case ';':
	case ',':
	case '.':
		out_begin = in_begin;
		out_end = in_begin + 1;
		return true;
	}

	return false;
}

void TextEditWindow::SetDeclsCS() {
	TextEditor::LanguageDefinition lang;
	lang.mCommentStart = "/*";
	lang.mCommentEnd = "*/";
	lang.mSingleLineComment = "//";
	lang.mCaseSensitive = true;
	lang.mAutoIndentation = true;

	lang.mName = "C#";
	static const char* const cppKeywords[] = {
			"using", "if", "while", "for", "foreach", "new", "return",
			"break", "continue", "class", "interface", "abstract", "static", "public",
			"private", "this", "protected", "virtual", "void", "int", "float", "double",
			"string", "uint", "bool", "event", "delegate", "async", "await", "sealed", "partial"
	};
	for (auto& k : cppKeywords)
		lang.mKeywords.insert(k);
	

	const std::vector<VSGE::MonoClassDesc*>& api_classes = 
		MonoScriptStorage::Get()->GetApiDescriptors()->GetClassesDescs();

	const std::vector<VSGE::MonoClassDesc*>& user_classes =
		VSGE::MonoScriptingLayer::Get()->GetScriptsBlob()->GetClassesDescs();

	for (auto api_class : api_classes) {
		TextEditor::Identifier id;
		id.mDeclaration = "VSGE API internal class";
		lang.mIdentifiers.insert(std::make_pair(std::string(api_class->GetName()), id));
	}

	for (auto user_class : user_classes) {
		TextEditor::Identifier id;
		id.mDeclaration = "user-defined class";
		lang.mIdentifiers.insert(std::make_pair(std::string(user_class->GetName()), id));
	}

	lang.mTokenize = [](const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, TextEditor::PaletteIndex& paletteIndex) -> bool
	{
		paletteIndex = TextEditor::PaletteIndex::Max;

		while (in_begin < in_end && isascii(*in_begin) && isblank(*in_begin))
			in_begin++;

		if (in_begin == in_end)
		{
			out_begin = in_end;
			out_end = in_end;
			paletteIndex = TextEditor::PaletteIndex::Default;
		}
		else if (TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
			paletteIndex = TextEditor::PaletteIndex::String;
		else if (TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
			paletteIndex = TextEditor::PaletteIndex::CharLiteral;
		else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
			paletteIndex = TextEditor::PaletteIndex::Identifier;
		else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
			paletteIndex = TextEditor::PaletteIndex::Number;
		else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
			paletteIndex = TextEditor::PaletteIndex::Punctuation;

		return paletteIndex != TextEditor::PaletteIndex::Max;
	};
	_text_editor->SetLanguageDefinition(lang);
	TextEditor::LanguageDefinition::CPlusPlus();
}

void TextEditWindow::Draw() {
	ImGui::Begin(_win_title.c_str(), &_closed, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
	
	ImVec2 cur_size = ImGui::GetWindowSize();
	if (cur_size.x < 400 || cur_size.y < 300)
		ImGui::SetWindowSize(ImVec2(400, 300));

	DrawMenu();

	_text_editor->Render("ed");

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
		
		divider = str_line.size() - divider;
		str_line = str_line.substr(divider + _text_editor->GetTabSize() + 1);
	}

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