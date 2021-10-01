using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class AnimatorComponent : IEntityComponent {
    public AnimatorComponent(ulong handle) : base(handle)
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

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Play(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Stop(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsPlaying(ulong handle);
}