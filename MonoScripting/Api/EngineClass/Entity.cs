using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Entity : InternalObject {

    public Entity(ulong handle) : base(handle)
    {
    }

    public void SetActive(bool active){
        i_SetActive(handle, active);
    }

    bool IsActive(){
        return i_IsActive(handle);
    }

    public void SetName(string name){
        i_SetName(handle, name);
    }

    public string GetName(){
        return i_GetName(handle);
    }

    public Entity GetParent(){
        return new Entity(i_GetParent(handle));
    }

    public void AddChild(Entity entity){
        i_AddChild(handle, entity.handle);
    }

    public void RemoveChild(Entity entity){
        i_RemoveChild(handle, entity.handle);
    }

    public Vec3 GetPosition(){
        return i_GetPosition(handle);
    }

    public void SetPosition(Vec3 position){
        i_SetPosition(handle, position);
    }

    public Vec3 GetScale(){
        return i_GetScale(handle);
    }

    public void SetScale(Vec3 scale){
        i_SetScale(handle, scale);
    }

    public Quat GetRotation(){
        return i_GetRotation(handle);
    }

    public void SetRotation(Quat rotation){
        i_SetRotation(handle, rotation);
    }

    //INTERNALS

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_GetParent(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_AddChild(ulong handle, ulong child_handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_RemoveChild(ulong handle, ulong child_handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetActive(ulong handle, bool active);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsActive(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string i_GetName(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetName(ulong handle, string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_GetPosition(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetPosition(ulong handle, Vec3 position);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_GetScale(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_SetScale(ulong handle, Vec3 scale);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Quat i_GetRotation(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Quat i_SetRotation(ulong handle, Quat rotation);
}