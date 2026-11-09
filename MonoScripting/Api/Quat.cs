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

    public Quat Conjugate()
    {
        return new Quat(-x, -y, -z, w);
    }

    public Quat Inverse()
    {
        float length2 = (x * x) + (y * y) + (z * z) + (w * w);

        // Division by 0 protection
        if (length2 <= 0.000001f)
        {
            return new Quat(0.0f, 0.0f, 0.0f, 1.0f);
        }

        float invLength2 = 1.0f / length2;
        return new Quat(-x * invLength2, -y * invLength2, -z * invLength2, w * invLength2);
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

    public static Quat LookRotation(Vec3 forward, Vec3 up)
    {
        // Forward (Z axis)
        Vec3 zAxis = forward.GetNormalized();

        // Right (X axis)
        Vec3 xAxis = up.Cross(zAxis).GetNormalized();

        // UP (Y axis)
        Vec3 yAxis = zAxis.Cross(xAxis);

        float trace = xAxis.x + yAxis.y + zAxis.z;
        float x, y, z, w;

        if (trace > 0.0f)
        {
            float s = MathF.Sqrt(trace + 1.0f) * 2.0f;
            w = 0.25f * s;
            x = (zAxis.y - yAxis.z) / s;
            y = (xAxis.z - zAxis.x) / s;
            z = (yAxis.x - xAxis.y) / s;
        }
        else if ((xAxis.x > yAxis.y) && (xAxis.x > zAxis.z))
        {
            float s = MathF.Sqrt(1.0f + xAxis.x - yAxis.y - zAxis.z) * 2.0f;
            w = (zAxis.y - yAxis.z) / s;
            x = 0.25f * s;
            y = (xAxis.y + yAxis.x) / s;
            z = (zAxis.x + xAxis.z) / s;
        }
        else if (yAxis.y > zAxis.z)
        {
            float s = MathF.Sqrt(1.0f + yAxis.y - xAxis.x - zAxis.z) * 2.0f;
            w = (xAxis.z - zAxis.x) / s;
            x = (xAxis.y + yAxis.x) / s;
            y = 0.25f * s;
            z = (yAxis.z + zAxis.y) / s;
        }
        else
        {
            float s = MathF.Sqrt(1.0f + zAxis.z - xAxis.x - yAxis.y) * 2.0f;
            w = (yAxis.x - xAxis.y) / s;
            x = (zAxis.x + xAxis.z) / s;
            y = (yAxis.z + zAxis.y) / s;
            z = 0.25f * s;
        }

        return new Quat(x, y, z, w);
    }
}