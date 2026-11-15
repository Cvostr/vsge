#include "TextView.hpp"
#include "../UiLayer.hpp"
#include <Graphics/FontManager.hpp>

using namespace VSGE;

TextView::TextView() {
	_font_name = "arial";
	_text = "";
}
TextView::~TextView() {

}

void TextView::SetText(const String& text) {
	_text = text;
}

void TextView::SetFontName(const std::string& font_name) {
	_font_name = font_name;
}

void TextView::SetTextColor(const Color& color) {
	_text_color = color;
}

void TextView::Draw() 
{
	GlyphManager* glyph_manager = GlyphManager::Get();
	GlyphFontContainer* font = glyph_manager->GetFontByName(_font_name);

	// Actual pix size of string
	Vec2i stringPixSize = font->GetSizeOfString((uint32*)_text.c_str(), _text.Length());
	if (stringPixSize.x <= 0 || stringPixSize.y <= 0) {
		return;
	}

	float scaleX = _global_bounds.Size.x / (float)stringPixSize.x;
	float scaleY = _global_bounds.Size.y / (float)stringPixSize.y;

	// Take a lesser coeff
	float scale = std::min(scaleX, scaleY);
	scale = std::min(scale, 1.0f);

	Rect actualBounds;
	// Fit it into TextView bounds
	actualBounds.Size.x = stringPixSize.x * scale;
	actualBounds.Size.y = stringPixSize.y * scale;
	// Make it render at the center
	actualBounds.Pos.x = _global_bounds.Pos.x + (_global_bounds.Size.x - actualBounds.Size.x) * 0.5f;
	actualBounds.Pos.y = _global_bounds.Pos.y + (_global_bounds.Size.y - actualBounds.Size.y) * 0.5f;

	// Render text
	UiLayer::Get()->GetRenderList()->DrawText(actualBounds, _rotation, _text, _font_name, _text_color);
}