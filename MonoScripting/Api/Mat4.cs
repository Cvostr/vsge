public struct Mat4
{
    public Mat4(Vec4 column0, Vec4 column1, Vec4 column2, Vec4 column3)
    {
        this.m00 = column0.x; this.m10 = column0.y; this.m20 = column0.z; this.m30 = column0.w;
        this.m01 = column1.x; this.m11 = column1.y; this.m21 = column1.z; this.m31 = column1.w;
        this.m02 = column2.x; this.m12 = column2.y; this.m22 = column2.z; this.m32 = column2.w;
        this.m03 = column3.x; this.m13 = column3.y; this.m23 = column3.z; this.m33 = column3.w;
    }

    // Column 0
    public float m00;
    public float m10;
    public float m20;
    public float m30;

    // Column 1
    public float m01;
    public float m11;
    public float m21;
    public float m31;

    // Column 2
    public float m02;
    public float m12;
    public float m22;
    public float m32;

    // Column 3
    public float m03;
    public float m13;
    public float m23;
    public float m33;
}