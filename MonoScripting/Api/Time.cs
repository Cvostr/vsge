using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class TimePerf{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern float GetDeltaTime();  

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern ulong GetTicks();  
}