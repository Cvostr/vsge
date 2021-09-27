using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class MeshComponent : IEntityComponent {
    public MeshComponent(ulong handle) : base(handle)
    {
    }
}