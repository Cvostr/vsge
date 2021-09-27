using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class AudioSourceComponent : IEntityComponent {
    public AudioSourceComponent(ulong handle) : base(handle)
    {
    }
}