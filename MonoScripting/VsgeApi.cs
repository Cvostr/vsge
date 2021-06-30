using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using ObjectPtr = System.UInt64;

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
}

public struct Quat
{
    public float x;
    public float y;
    public float z;
    public float w;
}

public class Entity
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern void SetName(string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern string GetName();

    private ObjectPtr handle;
}