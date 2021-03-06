using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class ColliderComponent : IEntityComponent {
    public ColliderComponent(ulong handle) : base(handle)
    {
    }

    bool IsTrigger(){
        return i_IsTrigger(handle);
    }

    void SetTrigger(bool trigger){
        i_SetTrigger(handle, trigger);
    }

    Vec3 GetSize(){
        return i_GetSize(handle);
    }

    Vec3 GetCenter(){
        return i_GetCenter(handle);
    }

    void SetSize(Vec3 size){
        i_SetSize(handle, size);
    }

    void SetCenter(Vec3 center){
        i_SetCenter(handle, center);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsTrigger(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetTrigger(ulong handle, bool trigger);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_GetSize(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_GetCenter(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetSize(ulong handle, Vec3 size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetCenter(ulong handle, Vec3 center);
}