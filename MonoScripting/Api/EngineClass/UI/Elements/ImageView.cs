using System;
using System.Reflection;

public class ImageView : View
{
    private Resource _sprite;
    private Color _blendColor;

    public ImageView() : base()
    {
        _sprite = null;
        _blendColor = new Color(1, 1, 1, 1);
    }

    public void SetSprite(Resource sprite)
    {
        _sprite = sprite;
    }

    public void SetBlendColor(Color color)
    {
        _blendColor = color;
    }

    public override void Render()
    {
        if (_sprite == null)
            return;

        UiRenderList.DrawSprite(_globalBounds, 0, _sprite, new Vec2(0, 0), new Vec2(1, 1));
    }
}