using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class InternalObject{

    public InternalObject(ulong handle){
        this.handle = handle;
    }

    protected ulong handle;
}