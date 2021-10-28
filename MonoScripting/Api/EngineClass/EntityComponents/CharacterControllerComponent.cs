using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class CharacterControllerComponent : IEntityComponent {
    public CharacterControllerComponent(ulong handle) : base(handle)
    {
    }

    public CharacterControllerComponent() : base(0)
    {
    }

    public bool IsOnGround(){
        return i_IsOnGround(handle);
    }

    public void Move(Vec2 direction){
        i_Move(handle, direction);
    }
    
    public Vec3 GetCenter(){
        return i_GetCenter(handle);
    }

    public Vec2 GetSize(){
        return i_GetSize(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsOnGround(ulong handle);
   
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_Move(ulong handle, Vec2 dir);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec3 i_GetCenter(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern Vec2 i_GetSize(ulong handle);
}