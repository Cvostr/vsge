#include "../ApiBindings.hpp"
#include <UI/UiLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

void DrawSprite(Rect bounds, float rotation, Resource* sprite, Vec2 uv_min, Vec2 uv_max) {

}

void DrawText(Rect bounds, float rotation, MonoString* text, MonoString* font, Color color) {
	String text32 = String(mono_string_to_utf32(text));
	std::string font_name = std::string(mono_string_to_utf8(font));
	UiLayer::Get()->GetRenderList()->DrawText(bounds, rotation, text32, font_name, color);
}

void VSGE::BindUiRenderList() {
	MonoScriptingLayer::AddInternalCall("UiRenderList::DrawText(Rect,single,string,string,Color)", DrawText);
	MonoScriptingLayer::AddInternalCall("UiRenderList::DrawText(Rect,single,ulong,Vec2,Vec2)", DrawSprite);
}