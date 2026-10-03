public struct Vec4 
{
    public Vec4(float x, float y, float z, float w)
    {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = w;
    }

    public Vec4(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = 0.0f;
    }

    public bool Equals(Vec4 other)
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    public float x;
    public float y;
    public float z;
    public float w;
}