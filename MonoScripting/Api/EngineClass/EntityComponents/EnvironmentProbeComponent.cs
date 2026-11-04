using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class EnvironmentProbeComponent : IEntityComponent
{

    public EnvironmentProbeComponent() : base(0)
    {
    }

    public EnvironmentProbeComponent(ulong handle) : base(handle)
    {
    }

    public void Render()
    {
        i_Render(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Render(ulong handle);
}