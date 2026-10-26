using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class InternalObject{

    public InternalObject(ulong handle)
    {
        this.handle = handle;
    }

    public InternalObject()
    {
        this.handle = 0;
    }

    public void SetHandle(ulong handle)
    {
        this.handle = handle;
    }

    public ulong GetHandle()
    {
        return handle;
    }
    
    public static bool operator ==(InternalObject? left, InternalObject? right)
    {
        if (left is null)
        {
            return right is null;
        }

        if (right is null)
        {
            return false;
        }

        return left.handle == right.handle;
    }
    
    public static bool operator !=(InternalObject? left, InternalObject? right)
    {
        return !(left == right);
    }

    protected ulong handle;
}