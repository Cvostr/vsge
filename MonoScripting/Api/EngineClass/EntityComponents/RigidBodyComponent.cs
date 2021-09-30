using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class RigidbodyComponent : IEntityComponent {
    public RigidbodyComponent(ulong handle) : base(handle)
    {
    }
}