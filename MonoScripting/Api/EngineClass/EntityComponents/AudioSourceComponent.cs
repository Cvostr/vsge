using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class AudioSourceComponent : IEntityComponent {
    public AudioSourceComponent(ulong handle) : base(handle)
    {
    }

    public void Play(){
        i_Play(handle);
    }
    public void Stop(){
        i_Stop(handle);
    }
    public void Pause(){
        i_Pause(handle);
    }

    public float GetVolume(){
        return i_GetVolume(handle);
    }

    public void SetVolume(float volume){
        i_SetVolume(handle, volume);
    }

    public float GetPitch(){
        return i_GetPitch(handle);
    }

    public void SetPitch(float pitch){
        i_SetPitch(handle, pitch);
    }

    public bool IsLooping(){
        return i_IsLooping(handle);
    }

    public void SetLooping(bool loop){
        i_SetLooping(handle, loop);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Play(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Stop(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Pause(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetVolume(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetVolume(ulong handle, float volume);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetPitch(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetPitch(ulong handle, float pitch);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsLooping(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetLooping(ulong handle, bool loop);
}