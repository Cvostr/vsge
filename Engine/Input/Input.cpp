#include "Input.hpp"

using namespace VSGE;

Input* Input::_this = new Input;

Input::Input() {
	_pressed_key_queue = new KeyCode[KEY_QUEUE_MAX_SIZE];
	_pressed_key_count = 0;

	_hold_key_queue = new KeyCode[KEY_QUEUE_MAX_SIZE];
	_hold_key_count = 0;
}
Input::~Input() {
	SAFE_RELEASE_ARR(_pressed_key_queue);
	SAFE_RELEASE_ARR(_hold_key_queue);
}

Input* Input::Get() {
	return _this;
}

const MouseCursorState& Input::GetMouseCursorState() {
	return _mouse_cursor_state;
}
const MouseButtonState& Input::GetMouseButtonState() {
	return _mouse_btn_state;
}
bool Input::IsKeyPressed(KeyCode key) {
	for (uint32 i = 0; i < _pressed_key_count; i++) {
		if (_pressed_key_queue[i] == key)
			return true;
	}

	return false;
}
bool Input::IsKeyHold(KeyCode key) {
	for (uint32 i = 0; i < _hold_key_count; i++) {
		if (_hold_key_queue[i] == key)
			return true;
	}

	return false;
}

void Input::SetMouseCursorState(uint32 posX, uint32 posY, uint32 relX, uint32 relY) {
	_mouse_cursor_state.cursor_pos = Vec2i(posX, posY);
	_mouse_cursor_state.cursor_delta = Vec2i(relX, relY);
}
void Input::SetMouseBtnPressed(MouseButton btn) {
	switch (btn) {
	case MOUSE_BUTTON_LEFT:
		_mouse_btn_state.leftbtn_pressed = true;
		break;
	case MOUSE_BUTTON_MIDDLE:
		_mouse_btn_state.midbtn_pressed = true;
		break;
	case MOUSE_BUTTON_RIGHT:
		_mouse_btn_state.rightbtn_pressed = true;
	}
}
void Input::UnsetPressedMouseButtons() {
	_mouse_btn_state.leftbtn_pressed = false;
	_mouse_btn_state.rightbtn_pressed = false;
	_mouse_btn_state.midbtn_pressed = false;
}
void Input::SetMouseBtnHold(MouseButton btn) {
	switch (btn) {
	case MOUSE_BUTTON_LEFT:
		_mouse_btn_state.leftbtn_hold = true;
		break;
	case MOUSE_BUTTON_MIDDLE:
		_mouse_btn_state.midbtn_hold = true;
		break;
	case MOUSE_BUTTON_RIGHT:
		_mouse_btn_state.rightbtn_hold = true;
	}
}
void Input::UnsetMouseBtnHold(MouseButton btn) {
	switch (btn) {
	case MOUSE_BUTTON_LEFT:
		_mouse_btn_state.leftbtn_hold = false;
		break;
	case MOUSE_BUTTON_MIDDLE:
		_mouse_btn_state.midbtn_hold = false;
		break;
	case MOUSE_BUTTON_RIGHT:
		_mouse_btn_state.rightbtn_hold = false;
	}
}

void Input::AddHoldKey(KeyCode key) {
	if (!IsKeyHold(key) && _hold_key_count < KEY_QUEUE_MAX_SIZE) {
		_hold_key_queue[_hold_key_count++] = key;
	}
}
void Input::AddPressedKey(KeyCode key) {
	if (!IsKeyPressed(key) && _pressed_key_count < KEY_QUEUE_MAX_SIZE) {
		_pressed_key_queue[_pressed_key_count++] = key;
	}
}
void Input::ClearPressedKeys() {
	_pressed_key_count = 0;
}
void Input::RemoveHoldKey(KeyCode key) {
	for (uint32 i = 0; i < _hold_key_count; i++) {
		if (_hold_key_queue[i] == key) {
			for (uint32 j = i + 1; j < _hold_key_count; j ++) {
				_hold_key_queue[j - 1] = _hold_key_queue[j];
			}
			_hold_key_count--;
			break;
		}
	}
}