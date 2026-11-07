#include "../ApiBindings.hpp"
#include <UI/UiLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

void DrawSprite(Rect bounds, float rotation, Resource* sprite, Vec2 uv_min, Vec2 uv_max) 
{
	if (sprite->GetResourceType() != RESOURCE_TYPE_TEXTURE)
		return;

	UiLayer::Get()->GetRenderList()->DrawSprite(bounds, rotation, (TextureResource*) sprite, uv_min, uv_max);
}

void DrawText(Rect bounds, float rotation, MonoString* text, MonoString* font, Color color) 
{
	mono_unichar4* pText = mono_string_to_utf32(text);
	char* pFont = mono_string_to_utf8(font);

	String text32 = String(pText);
	mono_free(pText);

	std::string fontName = std::string(pFont);
	mono_free(pFont);

	UiLayer::Get()->GetRenderList()->DrawText(bounds, rotation, text32, fontName, color);
}

void VSGE::BindUiRenderList() {
	MonoScriptingLayer::AddInternalCall("UiRenderList::DrawText(Rect,single,string,string,Color)", DrawText);
	MonoScriptingLayer::AddInternalCall("UiRenderList::i_DrawSprite(Rect,single,ulong,Vec2,Vec2)", DrawSprite);
}