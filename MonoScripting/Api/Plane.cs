using System;
using System.Runtime.InteropServices;

public struct Plane
{
    public Plane(Vec3 normal, float d)
    {
        this.normal = normal;
        this.d = d;
    }

    public Plane(Vec3 normal, Vec3 point)
    {
	    this.normal = normal;
        this.normal.Normalize();
	    this.d = (this.normal * -1).Dot(point);
    }

    public Plane()
    {
        d = 0;
    }

    public Vec3 normal;
    public float d;
}