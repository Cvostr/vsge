#include "ApiBindings.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Networking/Server.hpp>
#include <Networking/Client.hpp>

#include <Networking/EnetDrv/Server.hpp>
#include <Networking/EnetDrv/Client.hpp>

using namespace VSGE;

static void SubscribeToNetworkEvent(MonoObject* obj, EventType event_type, MonoString* method_name) {
	std::string method_name_str = std::string(mono_string_to_utf8(method_name));
	MonoScriptingLayer::Get()->SubscribeToEvent(obj, event_type, method_name_str);
}
static uint32 GetClientId() {
	return MonoScriptingLayer::Get()->GetNetworkEventState()._client_id;
}
static void* GetClientHandle() {
	return MonoScriptingLayer::Get()->GetNetworkEventState().client_ptr;
}
static void* GetServerHandle() {
	return MonoScriptingLayer::Get()->GetNetworkEventState().server_ptr;
}
static uint32 GetDataSize() {
	return MonoScriptingLayer::Get()->GetNetworkEventState().data_size;
}
static MonoArray* GetData() {
	return MonoScriptingLayer::Get()->GetNetworkEventState().data;
}

static IGameServer* CreateServer(int driver) {
	if (driver == 0)
		return new EnetGameServer;
}
static void StartServer(IGameServer* server) {
	server->StartServer();
}
static void StopServer(IGameServer* server) {
	server->StopServer();
}
static void SetServerPort(IGameServer* server, uint16 port) {
	server->SetServerPort(port);
}

static IGameClient* CreateClient(int driver) {
	if (driver == 0)
		return new EnetGameClient;
	return nullptr;
}
static bool ClientConnect(IGameClient* client, MonoString* address, uint16 port) {
	std::string address_std = std::string(mono_string_to_utf8(address));
	return client->Connect(address_std, port);
}
static void ClientDisconnect(IGameClient* client) {
	return client->Disconnect();
}
static void ClientSendPacket(IGameClient* client, MonoArray* mono_array, bool reliable) {
	uint32 size = mono_array_length(mono_array);
	byte* cpp_array = new byte[size];
	for (uint32 i = 0; i < size; i++) {
		cpp_array[i] = mono_array_get(mono_array, byte, i);
	}
	client->SendPacket(cpp_array, size, reliable);
}

void VSGE::BindNetworking() {
	MonoScriptingLayer::AddInternalCall(
		"GameNetworking::SubscribeToNetworkEvent(EntityScript,NetworkEventType,string)",
		SubscribeToNetworkEvent);
	MonoScriptingLayer::AddInternalCall(
		"GameNetworking::GetClientID()",
		GetClientId);
	MonoScriptingLayer::AddInternalCall(
		"GameNetworking::GetClientHandle()",
		GetClientHandle);
	MonoScriptingLayer::AddInternalCall(
		"GameNetworking::GetServerHandle()",
		GetServerHandle);
	MonoScriptingLayer::AddInternalCall(
		"GameNetworking::GetDataSize()",
		GetDataSize);
	MonoScriptingLayer::AddInternalCall(
		"GameNetworking::GetData()",
		GetData);


	MonoScriptingLayer::AddInternalCall(
		"GameServer::i_Create(GameNetworkingDriver)", CreateServer);
	MonoScriptingLayer::AddInternalCall(
		"GameServer::i_Start(ulong)", StartServer);
	MonoScriptingLayer::AddInternalCall(
		"GameServer::i_Stop(ulong)", StopServer);
	MonoScriptingLayer::AddInternalCall(
		"GameServer::i_SetPort(ulong,uint16)", SetServerPort);

	MonoScriptingLayer::AddInternalCall(
		"GameClient::i_Create(GameNetworkingDriver)", CreateClient);
	MonoScriptingLayer::AddInternalCall(
		"GameClient::i_Connect(ulong,string,uint16)", ClientConnect);
	MonoScriptingLayer::AddInternalCall(
		"GameClient::i_Disconnect()", ClientDisconnect);
	MonoScriptingLayer::AddInternalCall(
		"GameClient::i_SendPacket(ulong,byte[],bool)", ClientSendPacket);
}