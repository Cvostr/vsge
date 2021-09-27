using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class IEntityComponent : InternalObject {

    public IEntityComponent(ulong handle) : base(handle)
    {
    }

    Entity GetEntity(){
        return new Entity(i_GetEntity(handle));
    }

    bool IsActive(){
        return i_IsActive(handle);
    }

    void SetActive(bool active){
        i_SetActive(handle, active);
    }

    int GetType(){
        return i_GetType(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetEntity(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsActive(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetActive(ulong handle, bool active);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern int i_GetType(ulong handle);

}