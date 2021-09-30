using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class ColliderComponent : IEntityComponent {
    public ColliderComponent(ulong handle) : base(handle)
    {
    }
}