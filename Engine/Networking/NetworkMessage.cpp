#include "NetworkMessage.hpp"

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <enet/enet.h>
#undef _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace VSGE;

void VSGE::Enet_SendPacket(void* peer, byte* data, uint32 size) {
    ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE;
    ENetPacket* packet = enet_packet_create(data, size, flag);
    enet_peer_send((ENetPeer*)peer, 0, packet);
}