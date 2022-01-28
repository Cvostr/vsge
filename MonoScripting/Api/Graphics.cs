using System;
using System.Runtime.CompilerServices;

class IBL{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetEnvmapStepsCount(uint steps);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetIrmapStepsCount(uint steps);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetSpmapStepsCount(uint steps);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SetAlternateMode(bool alternate);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Update();
}