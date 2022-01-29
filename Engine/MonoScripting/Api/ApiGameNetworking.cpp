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
}
static bool ClientConnect(IGameClient* client, MonoString* address, uint16 port) {
	std::string address_std = std::string(mono_string_to_utf8(address));
	return client->Connect(address_std, port);
}
static void ClientDisconnect(IGameClient* client) {
	return client->Disconnect();
}

void VSGE::BindNetworking() {
	MonoScriptingLayer::AddInternalCall(
		"GameNetworking::SubscribeToNetworkEvent(EntityScript,NetworkEventType,string)",
		SubscribeToNetworkEvent);

	MonoScriptingLayer::AddInternalCall(
		"GameServer::i_Create(GameNetworkingDriver)", CreateServer);
	MonoScriptingLayer::AddInternalCall(
		"GameServer::i_Start(ulong)", StartServer);
	MonoScriptingLayer::AddInternalCall(
		"GameServer::i_Stop(ulong)", StopServer);
	MonoScriptingLayer::AddInternalCall(
		"GameServer::i_SetPort(ulong,ushort)", SetServerPort);

	MonoScriptingLayer::AddInternalCall(
		"GameClient::i_Create(GameNetworkingDriver)", CreateClient);
	MonoScriptingLayer::AddInternalCall(
		"GameClient::i_Connect(string,ushort)", ClientConnect);
	MonoScriptingLayer::AddInternalCall(
		"GameClient::i_Disconnect()", ClientDisconnect);
}