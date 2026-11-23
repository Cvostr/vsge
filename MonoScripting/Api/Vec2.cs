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

    void Normalize()
    {
        float len = Length();
        x /= len;
        y /= len;
    }

    public bool Equals(Vec2 other)
    {
        return x == other.x && y == other.y;
    }

    public static Vec2 operator *(Vec2 a, float b)
    {
        a.x *= b;
        a.y *= b;
        return a;
    }

    public static Vec2 operator +(Vec2 a, Vec2 b)
    {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    public static Vec2 operator -(Vec2 a, Vec2 b)
    {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }

    public static Vec2 operator *(Vec2 a, Vec2 b)
    {
        a.x *= b.x;
        a.y *= b.y;
        return a;
    }

    public override string ToString()
    {
        return $"({x}, {y})";
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

    public bool Equals(Vec2i other)
    {
        return x == other.x && y == other.y;
    }

    public override string ToString()
    {
        return $"({x}, {y})";
    }

    public int x;
    public int y;
}