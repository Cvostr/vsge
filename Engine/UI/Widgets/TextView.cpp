#include "TextView.hpp"
#include "../UiLayer.hpp"

using namespace VSGE;

TextView::TextView() {
	_font_name = "arial";
	_text = "test";
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

void TextView::Draw() {
	UiLayer::Get()->GetRenderList()->DrawText(_transform, _bounds, _text, _font_name, _text_color);
}