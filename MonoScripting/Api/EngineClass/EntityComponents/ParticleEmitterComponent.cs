using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class ParticleEmitterComponent : IEntityComponent {
    public ParticleEmitterComponent(ulong handle) : base(handle)
    {
    }

    public ParticleEmitterComponent() : base(0)
    {
    }

    public void StartSimulation(){
        i_StartSimulation(handle);
    }
    public void StopSimulation(){
        i_StopSimulation(handle);
    }
    public bool IsSimulating(){
        return i_IsSimulating(handle);
    }

    public float GetDuration(){
        return i_GetDuration(handle);
    }
    public void SetDuration(float duration){
        i_SetDuration(handle, duration);
    }

    public bool IsLooping(){
        return i_IsLooping(handle);
    }
    public void SetLooping(bool looping){
        i_SetLooping(handle, looping);
    }

    public bool IsPrewarm(){
        return i_IsPrewarm(handle);
    }
    public void SetPrewarm(bool prewarm){
        i_SetPrewarm(handle, prewarm);
    }

    public float GetParticleLifetime(){
        return i_GetParticleLifetime(handle);
    }
    public void SetParticleLifetime(float lifetime){
        i_SetParticleLifetime(handle, lifetime);
    }

    public uint GetMaxParticlesCount(){
        return i_GetMaxParticlesCount(handle);
    }
    public void SetMaxParticlesCount(uint max_particles){
        i_SetMaxParticlesCount(handle, max_particles);
    }

    public void SetEmissionRate(int min, int max){

    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_StartSimulation(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_StopSimulation(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsSimulating(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetDuration(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetDuration(ulong handle, float duration);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsLooping(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetLooping(ulong handle, bool looping);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_IsPrewarm(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetPrewarm(ulong handle, bool prewarm);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern float i_GetParticleLifetime(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetParticleLifetime(ulong handle, float lifetime);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint i_GetMaxParticlesCount(ulong handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetMaxParticlesCount(ulong handle, uint max_particles);
}