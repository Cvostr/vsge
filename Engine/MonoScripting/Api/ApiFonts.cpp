#include "ApiBindings.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>

#include <Graphics/FontManager.hpp>

using namespace VSGE;

static bool LoadFontFromFile(MonoString* filePath, MonoString* fontName) 
{
	char* pFilePath = mono_string_to_utf8(filePath);
	char* pFontName = mono_string_to_utf8(fontName);

	bool res = GlyphManager::Get()->AddFontContainer(std::string(pFilePath), std::string(pFontName));

	mono_free(pFilePath);
	mono_free(pFontName);

	return res;
}

static VSGE::GlyphFontContainer* GetFontByName(MonoString* fontName)
{
	char* pFontName = mono_string_to_utf8(fontName);

	VSGE::GlyphFontContainer* container = GlyphManager::Get()->GetFontByName(std::string(pFontName));
	
	mono_free(pFontName);

	return container;
}

static void GetSizeOfString(VSGE::GlyphFontContainer* font, MonoString* str, Vec2i* out)
{
	mono_unichar4* pText = mono_string_to_utf32(str);
	String text32 = String(pText);
	mono_free(pText);

	*out = font->GetSizeOfString(text32.c_str(), text32.Length());
}

void VSGE::BindFonts()
{
	MonoScriptingLayer::AddInternalCall("Fonts::LoadFontFromFile(string, string)", LoadFontFromFile);
	MonoScriptingLayer::AddInternalCall("Fonts::i_GetFontByName(string)", GetFontByName);

	MonoScriptingLayer::AddInternalCall("Font::i_GetSizeOfString(ulong,string,Vec2i&)", GetSizeOfString);
}