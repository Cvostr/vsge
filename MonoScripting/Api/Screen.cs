using System;
using System.Runtime.CompilerServices;

public enum FullscreenMode{
    MODE_WINDOWED,
    MODE_FULLSCREEN,
    MODE_FULLSCREEN_DESKTOP
}

public class Screen {
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetResolution(int width, int height);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetFullscreenMode(FullscreenMode mode);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetWindowResizeable(bool resizeable);
}