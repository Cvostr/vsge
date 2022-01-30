using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

public class GameClient : InternalObject {

    public GameClient(ulong handle) : base(handle)
    {
    }

    public GameClient(GameNetworkingDriver driver)
    {
        handle = i_Create(driver);
    }

    public bool Connect(string address, ushort port){
        return i_Connect(handle, address, port);
    }

    public void Disconnect(){
        i_Disconnect(handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_Create(GameNetworkingDriver driver);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool i_Connect(ulong handle, string address, ushort port);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Disconnect(ulong handle);
}