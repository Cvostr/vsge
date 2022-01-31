using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

public class GameServer : InternalObject {

    public GameServer(ulong handle) : base(handle)
    {
    }

    public GameServer(GameNetworkingDriver driver){
        handle = i_Create(driver);
    }

    public void SetMaxConnections(uint max){
        i_SetMaxConnections(handle, max);
    }

    public void SetPort(ushort port){
        i_SetPort(handle, port);
    }

    public void DisconnectClient(uint client_id){
        i_DisconnectClient(handle, client_id);
    }

    public void Start(){
        i_Start(handle);
    }

    public void Stop(){
        i_Stop(handle);
    }

    public void SendPacket(uint client_id, byte[] data, bool reliable){
        i_SendPacket(handle, client_id, data, reliable);
    } 

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern ulong i_Create(GameNetworkingDriver driver);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Start(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_Stop(ulong handle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetPort(ulong handle, ushort port);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SetMaxConnections(ulong handle, uint max);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_DisconnectClient(ulong handle, uint client_id);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void i_SendPacket(ulong handle, uint client_id, byte[] data, bool reliable);
}