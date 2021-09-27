using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class LightComponent : IEntityComponent {

    public LightComponent(ulong handle) : base(handle)
    {
    }

}