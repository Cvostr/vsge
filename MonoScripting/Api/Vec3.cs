using System;
using System.Runtime.InteropServices;

public struct Vec3
   {
    public Vec3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public float x;
    public float y;
    public float z;

    void Normalize(){
        float len = Length();
        x /= len;
        y /= len;
        z /= len;
    }

    public float Dot(Vec3 v){
        return (x * v.x) + (y * v.y) + (z * v.z);
    }

    public Vec3 Cross(Vec3 v){
        return new Vec3((y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x));
    }

    public float Length(){
        return (float)Math.Sqrt(x * x + y * y + z * z);
    }

    public static Vec3 operator *(Vec3 a, float b)
    {
        a.x *= b;
        a.y *= b;
        a.z *= b;
        return a;
    }

    public static Vec3 operator +(Vec3 a, Vec3 b)
    {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        return a;
    }

    public static Vec3 operator -(Vec3 a, Vec3 b)
    {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        return a;
    }

    public bool Equals(Vec3 other)
    {
        return x == other.x && y == other.y && z == other.z;
    }    

    public override string ToString()
    {
        return $"({x}, {y}, {z})";
    }
}