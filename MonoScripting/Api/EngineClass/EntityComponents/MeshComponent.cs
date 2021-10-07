using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class MeshComponent : IEntityComponent {
    public MeshComponent(ulong handle) : base(handle)
    {
    }

    void SetMesh(Resource resource){
        //i_SetAudioClip(handle, resource.GetHandle());
    }
}