public struct Vec4 
{
    public Vec4(float x, float y, float z, float w)
    {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = w;
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