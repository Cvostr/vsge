using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class ColliderComponent : IEntityComponent {
    public ColliderComponent(ulong handle) : base(handle)
    {
    }
}