using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

public class GameNetworking {
    public delegate void Server_ClientConnectedHandler(uint32 client_id);
    
    //[MethodImpl(MethodImplOptions.InternalCall)]
    //public static extern void SubscribeToNetworkEvent(EntityScript script_object, string method_name);
}