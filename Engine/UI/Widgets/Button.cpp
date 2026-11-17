#include "Button.hpp"
#include "../UiLayer.hpp"
#include <Input/Input.hpp>
#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Graphics/FontManager.hpp>

using namespace VSGE;

Button::Button() :
	_font_name("arial"),
	_state(ButtonState::NORMAL)
{

}
Button::~Button() {

}

void Button::SetText(const String& text) {
	_text = text;
}

void Button::SetCommonSprite(const std::string& sprite) {
	_normal_reference.SetResource(ResourceCache::Get()->GetResource(sprite));
}

void Button::SetHoveredSprite(const std::string& sprite) {
	_hovered_reference.SetResource(ResourceCache::Get()->GetResource(sprite));
}

void Button::SetHoldSprite(const std::string& sprite) {
	_hold_reference.SetResource(ResourceCache::Get()->GetResource(sprite));
}

bool Button::IsPressed() const {
	return _state == ButtonState::HOLD;
}

void Button::Draw() 
{
	UpdateState();

	GlyphManager* glyph_manager = GlyphManager::Get();
	GlyphFontContainer* font = glyph_manager->GetFontByName(_font_name);

	// Actual pix size of string
	Vec2i stringPixSize = font->GetSizeOfString((uint32*)_text.c_str(), _text.Length());
	if (stringPixSize.x <= 0 || stringPixSize.y <= 0) {
		return;
	}

	TextureResource* spriteToDraw = nullptr;
	
	switch (_state) {
		case ButtonState::NORMAL:
			spriteToDraw = _normal_reference.GetResource<TextureResource>();
			break;
		case ButtonState::HOVERED:
			spriteToDraw = _hovered_reference.GetResource<TextureResource>();
			break;
		case ButtonState::HOLD:
			spriteToDraw = _hold_reference.GetResource<TextureResource>();
			break;
	}

	if (spriteToDraw == nullptr)
		return;

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

	// Render sprite
	UiLayer::Get()->GetRenderList()->DrawSprite(_global_bounds, _rotation, spriteToDraw);
	// Render text
	UiLayer::Get()->GetRenderList()->DrawText(actualBounds, _rotation, _text, _font_name, Color(1,1,1,1));
}

void Button::SetOnClick(click_callback callback) {
	_callback = std::move(callback);
}

void Button::UpdateState()
{
	Vec2 cursorPos = UiLayer::Get()->GetMouseCursorPos();

	bool isInside =
		(cursorPos.x >= _global_bounds.Pos.x) && (cursorPos.x <= _global_bounds.Pos.x + _global_bounds.Size.x) &&
		(cursorPos.y >= _global_bounds.Pos.y) && (cursorPos.y <= _global_bounds.Pos.y + _global_bounds.Size.y);

	bool isMouseHold = Input::Get()->IsMouseButtonHold(MOUSE_BUTTON_LEFT);

	if (isInside) 
	{
		// Cursor is inside
		switch (_state) {
			case ButtonState::NORMAL:
				_state = ButtonState::HOVERED;
				break;
			case ButtonState::HOVERED:
				if (isMouseHold)
					_state = ButtonState::HOLD;
				break;
			case ButtonState::HOLD:
				if (!isMouseHold) {
					_state = ButtonState::HOVERED;
					if (_callback) {
						_callback();
					}
				}
				break;
		}
	}
	else
	{
		// cursor is outside
		switch (_state) {
			case ButtonState::HOVERED:
				_state = ButtonState::NORMAL;
				break;
			case ButtonState::HOLD:
				if (!isMouseHold) {
					_state = ButtonState::HOVERED;
					if (_callback) {
						_callback();
					}
				}
				break;
		}
	}
}