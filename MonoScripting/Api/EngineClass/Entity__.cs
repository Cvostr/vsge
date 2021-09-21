using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class Entity : InternalObject {

    public Entity(ulong handle) : base(handle)
    {
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static void SetActive(bool active);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static bool IsActive();
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static void SetName(string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static string GetName();
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static Entity GetParent();
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static void AddChild(Entity entity);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static void RemoveChild(Entity entity);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static Vec3 GetPosition();
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static void SetPosition(Vec3 position);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static Vec3 GetScale();
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static void SetScale(Vec3 scale);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static Quat GetRotation();
    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern static void SetRotation(Quat rotation);
}