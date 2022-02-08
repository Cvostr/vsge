#include "TextEditWindows.hpp"
#include <MonoScripting/MonoScriptStorage.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <EditorLayers/EditorLayer.hpp>

using namespace VSGEditor;

TextEditWindows* TextEditWindows::_this = new TextEditWindows;

TextEditWindows::TextEditWindows() {
	_this = this;
	cs_lang = nullptr;
}

TextEditWindows::~TextEditWindows() {
	for (auto it = text_edit_windows.begin(); it != text_edit_windows.end(); ++it) {
		delete (*it);
	}
	text_edit_windows.clear();
	delete cs_lang;
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

void TextEditWindows::CreateLangDefinition() {
	if (cs_lang) {
		delete cs_lang;
	}
	cs_lang = new TextEditor::LanguageDefinition;

	cs_lang->mCommentStart = "/*";
	cs_lang->mCommentEnd = "*/";
	cs_lang->mSingleLineComment = "//";
	cs_lang->mCaseSensitive = true;
	cs_lang->mAutoIndentation = true;

	cs_lang->mName = "C#";
	static const char* const cppKeywords[] = {
			"using", "if", "while", "for", "foreach", "new", "return",
			"break", "continue", "class", "interface", "abstract", "static", "public",
			"private", "this", "protected", "virtual", "void", "int", "float", "double",
			"string", "uint", "bool", "event", "delegate", "async", "await", "sealed", "partial"
	};
	for (auto& k : cppKeywords)
		cs_lang->mKeywords.insert(k);


	const std::vector<VSGE::MonoClassDesc*>& api_classes =
		MonoScriptStorage::Get()->GetApiDescriptors()->GetClassesDescs();

	const std::vector<VSGE::MonoClassDesc*>& user_classes =
		VSGE::MonoScriptingLayer::Get()->GetScriptsBlob()->GetClassesDescs();

	classes.clear();
	for (auto api_class : api_classes) {
		TextEditor::Identifier id;
		id.mDeclaration = "VSGE API internal class";
		cs_lang->mIdentifiers.insert(std::make_pair(std::string(api_class->GetName()), id));
		classes.push_back(api_class);
	}

	for (auto user_class : user_classes) {
		TextEditor::Identifier id;
		id.mDeclaration = "user-defined class";
		cs_lang->mIdentifiers.insert(std::make_pair(std::string(user_class->GetName()), id));
	}

	cs_lang->mTokenize = [](const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, TextEditor::PaletteIndex& paletteIndex) -> bool
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

	for (auto win : text_edit_windows) {
		win->SetDeclsCS();
	}
}

const TextEditor::LanguageDefinition& TextEditWindows::GetCsLangDefinition() {
	return *cs_lang;
}

std::vector<VSGE::MonoClassDesc*>& TextEditWindows::GetClassesDescs() {
	return classes;
}