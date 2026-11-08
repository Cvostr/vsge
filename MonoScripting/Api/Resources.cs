using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Resources
{
    public static Resource GetResource(string name)
    {
        ulong handle = i_GetResource(name, ResourceType.None);
        if (handle == 0)
            return null;
        return new Resource(handle);
    }

    public static Resource GetResource(string name, ResourceType type)
    {
        ulong handle = i_GetResource(name, type);
        if (handle == 0)
            return null;
        return new Resource(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern ulong i_GetResource(string name, ResourceType type);
}