using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

    public enum GameNetworkingDriver{
        GAME_NETWORKING_DRIVER_ENET,
        GAME_NETWORKING_DRIVER_VSNTWL
    }

public class GameNetworking {

    public enum NetworkEventType{
        EventNetworkClientConnected = 21,
		EventNetworkClientDisconnected = 22,
		EventNetworkServerDataReceive = 23,
		EventNetworkClientDataReceive = 24
    };

    public delegate void Server_ClientConnectedHandler(uint client_id);
    
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SubscribeToNetworkEvent(EntityScript script_object, NetworkEventType event_type, string method_name);
}