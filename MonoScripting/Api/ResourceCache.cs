using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class ResourceCache{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern Resource GetResource(string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern Resource[] GetAllResources();
}