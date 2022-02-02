using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

public enum GameNetworkingDriver{
    GAME_NETWORKING_DRIVER_ENET,
    GAME_NETWORKING_DRIVER_VSNTWL
}

public enum NetworkEventType{
        EventNetworkClientConnected = 21,
		EventNetworkClientDisconnected = 22,
		EventNetworkServerDataReceive = 23,
		EventNetworkClientDataReceive = 24,
        EventClientDisconnectedByServer = 25
    };

public class GameNetworking {

    public static GameClient GetClient(){
        return new GameClient(GetClientHandle());
    }

    public static GameServer GetServer(){
        return new GameServer(GetServerHandle());
    }

    public delegate void Server_ClientConnectedHandler(uint client_id);
    
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void SubscribeToNetworkEvent(EntityScript script_object, NetworkEventType event_type, string method_name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint GetClientID();

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong GetClientHandle();

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong GetServerHandle();

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint GetDataSize();

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern byte[] GetData();
}