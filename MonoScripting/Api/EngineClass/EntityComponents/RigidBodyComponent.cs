using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class RigidbodyComponent : IEntityComponent {
    public RigidbodyComponent(ulong handle) : base(handle)
    {
    }

    float GetMass(){
        return i_GetMass(handle);
    }

    void SetMass(float mass){
        i_SetMass(handle, mass);
    }

    float GetFriction(){
        return i_GetFriction(handle);
    }

    void SetFriction(float friction){
        i_SetFriction(handle, friction);
    }

    void ApplyCentralImpulse(Vec3 impulse){
        i_ApplyCentralImpulse(handle, impulse);
    }

    void ApplyCentralForce(Vec3 force){
        i_ApplyCentralForce(handle, force);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetMass(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetMass(ulong handle, float mass);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetFriction(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetFriction(ulong handle, float friction);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_ApplyCentralImpulse(ulong handle, Vec3 impulse);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_ApplyCentralForce(ulong handle, Vec3 force);
}