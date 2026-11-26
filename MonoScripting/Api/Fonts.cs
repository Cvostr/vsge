using System;
using System.Runtime.CompilerServices;

public class Fonts
{
    public static Font GetFont(string fontName)
    {
        ulong fontHandle = i_GetFontByName(fontName);
        if (fontHandle == 0)
            return null;

        return new Font(fontHandle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool LoadFontFromFile(string fontName, string filePath);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetFontByName(string fontName);
}