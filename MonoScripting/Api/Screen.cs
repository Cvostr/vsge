using System;
using System.Runtime.CompilerServices;

public enum FullscreenMode
{
    Windowed = 0,
    Fullscreen = 1,
    Borderless = 2
}

public class Screen 
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetResolution(int width, int height);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetFullscreenMode(FullscreenMode mode);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetResizable(bool resizable);
}