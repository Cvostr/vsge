using System;
using System.Runtime.CompilerServices;

public enum CursorMode
{
    Free = 0,		// Default
    Locked = 1,     // for FPS games
	Confined = 2    // for RTS games
};

public class Cursor
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetMode(CursorMode mode);
}