using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public enum MouseButton {
	MOUSE_BUTTON_LEFT = 1,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_RIGHT
};

public enum KeyCode {
	KEY_CODE_A = 'a',
	KEY_CODE_B = 'b',
	KEY_CODE_C = 'c',
	KEY_CODE_D = 'd',
	KEY_CODE_E = 'e',
	KEY_CODE_F = 'f',
	KEY_CODE_G = 'g',
	KEY_CODE_H = 'h',
	KEY_CODE_I = 'i',
	KEY_CODE_J = 'j',
	KEY_CODE_K = 'k',
	KEY_CODE_L = 'l',
	KEY_CODE_M = 'm',
	KEY_CODE_N = 'n',
	KEY_CODE_O = 'o',
	KEY_CODE_P = 'p',
	KEY_CODE_Q = 'q',
	KEY_CODE_R = 'r',
	KEY_CODE_S = 's',
	KEY_CODE_T = 't',
	KEY_CODE_U = 'u',
	KEY_CODE_V = 'v',
	KEY_CODE_W = 'w',
	KEY_CODE_X = 'x',
	KEY_CODE_Y = 'y',
	KEY_CODE_Z = 'z',

	KEY_CODE_F1,
	KEY_CODE_F2,
	KEY_CODE_F3,
	KEY_CODE_F4,
	KEY_CODE_F5,
	KEY_CODE_F6,
	KEY_CODE_F7,
	KEY_CODE_F8,
	KEY_CODE_F9,
	KEY_CODE_F10,
	KEY_CODE_F11,
	KEY_CODE_F12,

	KEY_CODE_LCTRL,
	KEY_CODE_RCTRL,
	KEY_CODE_TAB,
	KEY_CODE_LSHIFT,
	KEY_CODE_LALT,
	KEY_CODE_RALT,
	KEY_CODE_BACKSPACE,
	KEY_CODE_DELETE,
	KEY_CODE_SPACE,
	KEY_CODE_ESCAPE,
	KEY_CODE_ENTER,

	KEY_CODE_UP,
	KEY_CODE_DOWN,
	KEY_CODE_LEFT,
	KEY_CODE_RIGHT
};

public class Input{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern Vec2i GetMouseCursorPos();
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern Vec2i GetMouseCursorDelta();

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool IsMouseButtonPressed(MouseButton button);
    [MethodImpl(MethodImplOptions.InternalCall)]
	public static extern bool IsMouseButtonHold(MouseButton button);
    [MethodImpl(MethodImplOptions.InternalCall)]
	public static extern bool IsKeyPressed(KeyCode key);
    [MethodImpl(MethodImplOptions.InternalCall)]
	public static extern bool IsKeyHold(KeyCode key);
}