using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class CameraComponent : IEntityComponent {
    public CameraComponent(ulong handle) : base(handle)
    {
    }

    public float GetNearPlane(){
        return i_GetNearPlane(handle);
    }
    public void SetNearPlane(float near){
        i_SetNearPlane(handle, near);
    }

    public float GetFarPlane(){
        return i_GetFarPlane(handle);
    }
    public void SetFarPlane(float near){
        i_SetFarPlane(handle, near);
    }

    public float GetFov(){
        return i_GetFov(handle);
    }
    public void SetFov(float fov){
        i_SetFov(handle, fov);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetNearPlane(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetNearPlane(ulong handle, float near);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetFarPlane(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetFarPlane(ulong handle, float far);
   
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetFov(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetFov(ulong handle, float fov);
}