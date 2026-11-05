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

    public Quat Inverse()
    {
        return new Quat(-x, -y, -z, w);
    }

    public float Length()
    {
        return (float)Math.Sqrt(x * x + y * y + z * z + w * w);
    }

    public void Normalize()
    {
        float len = Length();
        x /= len;
        y /= len;
        z /= len;
        w /= len;
    }

    public Quat GetNormalized()
    {
        float len = Length();
        return new Quat(x / len, y / len, z / len, w / len);
    }

    public float Dot(Quat q)
    {
        return (x * q.x + y * q.y + z * q.z + w * q.w);
    }

    public static Quat operator *(Quat a, Quat b)
    {
        float num4 = b.x;
        float num3 = b.y;
        float num2 = b.z;
        float num = b.w;
        float num12 = (a.y * num2) - (a.z * num3);
        float num11 = (a.z * num4) - (a.x * num2);
        float num10 = (a.x * num3) - (a.y * num4);
        float num9 = ((a.x * num4) + (a.y * num3)) + (a.z * num2);
        return new Quat(
            ((a.x * num) + (num4 * a.w)) + num12,
            ((a.y * num) + (num3 * a.w)) + num11,
            ((a.z * num) + (num2 * a.w)) + num10,
            (a.w * num) - num9
        );
    }

    public static Quat FromEuler(float x, float y, float z)
    {
        float RADS = (MathF.PI / 180.0f);
        float pitch = x * RADS;
        float yaw = y * RADS;
        float roll = z * RADS;

        float halfRoll = roll * 0.5f;
        float halfPitch = pitch * 0.5f;
        float halfYaw = yaw * 0.5f;

        float sinRoll = MathF.Sin(halfRoll);
        float cosRoll = MathF.Cos(halfRoll);
        float sinPitch = MathF.Sin(halfPitch);
        float cosPitch = MathF.Cos(halfPitch);
        float sinYaw = MathF.Sin(halfYaw);
        float cosYaw = MathF.Cos(halfYaw);

        return new Quat(
            cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
            sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
            cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll,
            cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll
        );
    }
}