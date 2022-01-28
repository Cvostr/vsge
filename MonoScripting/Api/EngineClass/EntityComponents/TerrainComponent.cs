using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class TerrainComponent : IEntityComponent {
    public TerrainComponent(ulong handle) : base(handle)
    {
    }

    public TerrainComponent() : base(0)
    {
    }

    uint GetWidth(){
        return i_GetWidth(handle);
    }

    uint GetLength(){
        return i_GetLength(handle);
    }

    float GetMaxTerrainHeight(){
        return i_GetMaxTerrainHeight(handle);
    }

    void Flat(float height){
        i_Flat(handle, height);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Flat(ulong handle, float height);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint i_GetWidth(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint i_GetLength(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetMaxTerrainHeight(ulong handle);
}