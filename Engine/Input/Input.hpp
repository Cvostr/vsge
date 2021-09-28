#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Math/Vec2.hpp>
#include "InputConstants.hpp"

#define KEY_QUEUE_MAX_SIZE 10

namespace VSGE {
	struct MouseCursorState {
		Vec2i cursor_pos;
		Vec2i cursor_delta;

		MouseCursorState() {

		}
	};

	struct MouseButtonState {
		bool leftbtn_pressed;
		bool rightbtn_pressed;
		bool midbtn_pressed;

		bool leftbtn_hold;
		bool rightbtn_hold;
		bool midbtn_hold;

		MouseButtonState() {
			leftbtn_pressed = 0;
			rightbtn_pressed = 0;
			midbtn_pressed = 0;
			leftbtn_hold = 0;
			rightbtn_hold = 0;
			midbtn_hold = 0;
		}
	};

	class Input {
	private:
		static Input* _this;

		MouseCursorState _mouse_cursor_state;
		MouseButtonState _mouse_btn_state;

		KeyCode* _pressed_key_queue;
		uint32 _pressed_key_count;

		KeyCode* _hold_key_queue;
		uint32 _hold_key_count;
	public:
		Input();
		~Input();

		static Input* Get();

		const Vec2i& GetMouseCursorPos();
		const Vec2i& GetMouseCursorDelta();
		bool IsMouseButtonPressed(MouseButton button);
		bool IsMouseButtonHold(MouseButton button);
		bool IsKeyPressed(KeyCode key);
		bool IsKeyHold(KeyCode key);

		void SetMouseCursorState(uint32 posX, uint32 posY, uint32 relX, uint32 relY);
		void SetMouseBtnPressed(MouseButton btn);
		void UnsetPressedMouseButtons();
		void SetMouseBtnHold(MouseButton btn);
		void UnsetMouseBtnHold(MouseButton btn);

		void AddHoldKey(KeyCode key);
		void AddPressedKey(KeyCode key);

		void ClearPressedKeys();
		void RemoveHoldKey(KeyCode key);
	};
}