using System;

public class TextView : View
{
    private string _fontName;
    private string _text;
    private Color _textColor;

    private Vec2i _pixSize;

    public TextView() : base()
    {
        _fontName = "arial";
        _text = "";
        _textColor = new Color(1, 1, 1, 1);
    }

    public void SetText(string text) {
	    _text = text;
        UpdatePixSize();
    }

    public void SetTextColor(Color color) {
        _textColor = color;
    }

    public void SetFontName(string fontName) {
	    _fontName = fontName;
        UpdatePixSize();
    }

    private void UpdatePixSize()
    {
        Font font = Fonts.GetFont(_fontName);
        if (font != null)
        {
            _pixSize = font.GetSizeOfString(_text);
        }
    }

    public override void Render()
    {
        // Actual pix size of string
        if (_pixSize.x <= 0 || _pixSize.y <= 0)
        {
            return;
        }

        float scaleX = _globalBounds.Size.x / (float)_pixSize.x;
        float scaleY = _globalBounds.Size.y / (float)_pixSize.y;

        // Take a lesser coeff
        float scale = MathF.Min(scaleX, scaleY);
        scale = MathF.Min(scale, 1.0f);

        Rect actualBounds = new Rect(0, 0, 0, 0);
        // Fit it into TextView bounds
        actualBounds.Size.x = _pixSize.x * scale;
        actualBounds.Size.y = _pixSize.y * scale;
        // Make it render at the center
        actualBounds.Pos.x = _globalBounds.Pos.x + (_globalBounds.Size.x - actualBounds.Size.x) * 0.5f;
        actualBounds.Pos.y = _globalBounds.Pos.y + (_globalBounds.Size.y - actualBounds.Size.y) * 0.5f;

        // Render text
        UiRenderList.DrawText(actualBounds, 0, _text, _fontName, _textColor);
    }
}