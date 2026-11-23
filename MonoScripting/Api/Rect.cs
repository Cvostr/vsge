public struct Rect
{
    public Rect(Vec2 pos, Vec2 size)
    {
        this.Pos = pos;
        this.Size = size;
    }

    public Rect(float posX, float posY, float width, float height)
    {
        this.Pos.x = posX;
        this.Pos.y = posY;
        this.Size.x = width;
        this.Size.y = height;
    }

    public Vec2 GetCenter()
    {
        return Pos + Size * 0.5f;
    }

    public override string ToString()
    {
        return $"(x:{Pos.x}, y:{Pos.y}, width:{Size.x}, height:{Size.y})";
    }

    public Vec2 Pos;
    public Vec2 Size;
}
