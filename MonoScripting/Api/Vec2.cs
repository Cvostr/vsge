using System;

public struct Vec2
{
    public Vec2(float x, float y)
    {
        this.x = x;
        this.y = y;
    }

    public float Dot(Vec2 v)
    {
        return (x * v.x) + (y * v.y);
    }

    public float Length()
    {
        return (float) Math.Sqrt(x * x + y * y);
    }

    public bool Equals(Vec2 other)
    {
        return x == other.x && y == other.y;
    }

    public float x;
    public float y;
}

public struct Vec2i
{
    public Vec2i(int x, int y)
    {
        this.x = x;
        this.y = y;
    }

    public int x;
    public int y;
}