using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public enum MouseButton {
	Left = 1,
	Middle,
	Right
};

public enum KeyCode {
	A = 'a',
	B = 'b',
	C = 'c',
	D = 'd',
	E = 'e',
	F = 'f',
	G = 'g',
	H = 'h',
	I = 'i',
	J = 'j',
	K = 'k',
	L = 'l',
	M = 'm',
	N = 'n',
	O = 'o',
	P = 'p',
	Q = 'q',
	R = 'r',
	S = 's',
	T = 't',
	U = 'u',
	V = 'v',
	W = 'w',
	X = 'x',
	Y = 'y',
	Z = 'z',

	F1 = 123,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	LeftCtrl,
	RightCtrl,
	Tab,
	LeftShift,
	LeftAlt,
	RightAlt,
	Backspace,
	Delete,
	Space,
	Escape,
	Enter,

	Up,
	Down,
	Left,
	Right
};

public class Input
{
    public static Vec2i GetMouseCursorDelta() 
	{
        i_GetMouseCursorDelta(out Vec2i result);
        return result;
    }

    public static Vec2i GetMouseCursorPos()
    {
        i_GetMouseCursorPos(out Vec2i result);
        return result;
    }

    public static Vec2 GetMouseScroll()
	{
		i_GetMouseScroll(out Vec2 result);
		return result;
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_GetMouseCursorPos(out Vec2i res);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_GetMouseCursorDelta(out Vec2i res);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_GetMouseScroll(out Vec2 res);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool IsMouseButtonPressed(MouseButton button);

    [MethodImpl(MethodImplOptions.InternalCall)]
	public static extern bool IsMouseButtonHold(MouseButton button);

    [MethodImpl(MethodImplOptions.InternalCall)]
	public static extern bool IsKeyPressed(KeyCode key);

    [MethodImpl(MethodImplOptions.InternalCall)]
	public static extern bool IsKeyHold(KeyCode key);
}