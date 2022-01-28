using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class AnimatorComponent : IEntityComponent {
    public AnimatorComponent(ulong handle) : base(handle)
    {
    }

    public AnimatorComponent() : base(0)
    {
    }

    public void Play(){
        i_Play(handle);
    }
    public void Stop(){
        i_Stop(handle);
    }

    bool IsPlaying(){
        return i_IsPlaying(handle);
    }

    void AddAnimation(){
        i_AddAnimation(handle);
    }

    void SetAnimationCoeff(uint anim_index, float factor){
        i_SetAnimationCoeff(handle, anim_index, factor);
    }

    void SetAnimation(uint anim_index, Resource anim_resource){
        i_SetAnimation(handle, anim_index, anim_resource);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Play(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Stop(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsPlaying(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_AddAnimation(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetAnimationCoeff(ulong handle, uint anim_index, float factor);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetAnimation(ulong handle, uint anim_index, Resource anim_resource);
}