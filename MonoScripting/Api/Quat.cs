using System;

public struct Quat
   {
    public Quat(float x, float y, float z, float w)
    {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = w;
    }

    public float x;
    public float y;
    public float z;
    public float w;

    public Quat Inverse(){
        return new Quat(-x, -y, -z, w);
    }

    public float Length(){
        return (float)Math.Sqrt(x * x + y * y + z * z + w * w);
    }
}