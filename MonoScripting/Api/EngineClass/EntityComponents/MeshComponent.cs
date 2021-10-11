using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class MeshComponent : IEntityComponent {
    public MeshComponent(ulong handle) : base(handle)
    {
    }

    public MeshComponent() : base(0)
    {
    }

    void SetMesh(Resource resource){
        i_SetMesh(handle, resource.GetHandle());
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetMesh(ulong handle, ulong res_handle);
}