using System;

enum ButtonState
{
    Normal,
	Hovered,
	Hold
}

public class Button : View
{
    public event EventHandler Clicked;

    private string _fontName;
    private string _text;
    private Color _textColor;

    private Resource _normalSprite;
    private Resource _hoveredSprite;
    private Resource _holdSprite;

    private ButtonState _state;
    private Vec2i _pixSize;

    public Button() : base()
    {
        _fontName = "arial";
        _text = "";
        _textColor = new Color(1, 1, 1, 1);

        _normalSprite = null;
        _hoveredSprite = null;
        _holdSprite = null;

        // Reset default state
        _state = ButtonState.Normal;
    }

    public void SetText(string text)
    {
        _text = text;
        UpdatePixSize();
    }

    public void SetTextColor(Color color)
    {
        _textColor = color;
    }

    public void SetFontName(string fontName)
    {
        _fontName = fontName;
        UpdatePixSize();
    }

    public void SetCommonSprite(Resource sprite) 
    {
        _normalSprite = sprite;
    }

    public void SetHoveredSprite(Resource sprite) 
    {
        _hoveredSprite = sprite;
    }

    public void SetHoldSprite(Resource sprite) 
    {
        _holdSprite = sprite;
    }

    public override void Render()
    {
        UpdateState();

        // Actual pix size of string
        if (_pixSize.x <= 0 || _pixSize.y <= 0)
        {
            return;
        }

        Resource spriteToDraw = null;

        switch (_state)
        {
            case ButtonState.Normal:
                spriteToDraw = _normalSprite;
                break;
            case ButtonState.Hovered:
                spriteToDraw = _hoveredSprite;
                break;
            case ButtonState.Hold:
                spriteToDraw = _holdSprite;
                break;
        }

        if (spriteToDraw == null)
            return;

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

        // Render sprite
        UiRenderList.DrawSprite(_globalBounds, 0, spriteToDraw, new Vec2(0, 0), new Vec2(1, 1));
        // Render text
        UiRenderList.DrawText(actualBounds, 0, _text, _fontName, _textColor);
    }

    private void UpdatePixSize()
    {
        Font font = Fonts.GetFont(_fontName);
        if (font != null)
        {
            _pixSize = font.GetSizeOfString(_text);
        }
    }

    private void CallClickCallback()
    {
        Clicked?.Invoke(this, EventArgs.Empty);
    }

    private void UpdateState()
    {
        Vec2 cursorPos = UI.GetMouseCursorPos();

        bool isInside =
            (cursorPos.x >= _globalBounds.Pos.x) && (cursorPos.x <= _globalBounds.Pos.x + _globalBounds.Size.x) &&
            (cursorPos.y >= _globalBounds.Pos.y) && (cursorPos.y <= _globalBounds.Pos.y + _globalBounds.Size.y);

        bool isMouseHold = Input.IsMouseButtonHold(MouseButton.Left);

        if (isInside)
        {
            // Cursor is inside
            switch (_state)
            {
                case ButtonState.Normal:
                    _state = ButtonState.Hovered;
                    break;
                case ButtonState.Hovered:
                    if (isMouseHold)
                        _state = ButtonState.Hold;
                    break;
                case ButtonState.Hold:
                    if (!isMouseHold)
                    {
                        _state = ButtonState.Hovered;
                        CallClickCallback();
                    }
                    break;
            }
        }
        else
        {
            // cursor is outside
            switch (_state)
            {
                case ButtonState.Hovered:
                    _state = ButtonState.Normal;
                    break;
                case ButtonState.Hold:
                    if (!isMouseHold)
                    {
                        _state = ButtonState.Hovered;
                        CallClickCallback();
                    }
                    break;
            }
        }
    }
}