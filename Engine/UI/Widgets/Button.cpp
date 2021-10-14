#include "Button.hpp"

using namespace VSGE;

Button::Button() {
	_normal_reference.SetResourceType(RESOURCE_TYPE_TEXTURE);
	_hovered_reference.SetResourceType(RESOURCE_TYPE_TEXTURE);
	_pressed_reference.SetResourceType(RESOURCE_TYPE_TEXTURE);
}
Button::~Button() {

}

void Button::SetText(const String& text) {
	_text = text;
}

void Button::Draw() {

}