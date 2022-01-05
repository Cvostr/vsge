using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

enum UiRenderTaskType {
	UI_RENDER_TASK_TYPE_SPRITE,
	UI_RENDER_TASK_TYPE_TEXT
}

public class UiRenderList{

    public static void DrawSprite(Rect bounds, float rotation, Resource sprite, Vec2 uv_min, Vec2 uv_max){
        i_DrawSprite(bounds, rotation, sprite.GetHandle(), uv_min, uv_max);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_DrawSprite(Rect bounds, float rotation, ulong sprite, Vec2 uv_min, Vec2 uv_max);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void DrawText(Rect bounds, float rotation, string text, string font, Color color);
}