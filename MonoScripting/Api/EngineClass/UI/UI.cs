using System;
using System.ComponentModel.Design;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class UI
{
    private static Canvas _root = new Canvas();

    public static Canvas GetRootCanvas()
    {
        return _root;
    }

    public static void Render()
    {
        Vec2i res = UI.GetReferenceResolution();
        Rect screen = new Rect(0, 0, res.x, res.y);

        _root.UpdateGlobalRect(screen);
        _root.Process();
    }

    public static void SetReferenceResolution(int width, int height) 
    {
        i_SetReferenceResolution(width, height);
    }

    public static Vec2i GetReferenceResolution()
    {
        i_GetReferenceResolution(out Vec2i res);
        return res;
    }

    public static Vec2 GetMouseCursorPos()
    {
        i_GetMouseCursorPos(out Vec2 pos);
        return pos;
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_GetMouseCursorPos(out Vec2 res);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetReferenceResolution(int width, int height);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_GetReferenceResolution(out Vec2i res);
}