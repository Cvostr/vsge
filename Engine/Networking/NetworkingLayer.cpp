#include "NetworkingLayer.hpp"

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <enet/enet.h>
#undef _WINSOCK_DEPRECATED_NO_WARNINGS
#undef SendMessage

using namespace VSGE;

NetworkingLayer* NetworkingLayer::_this = nullptr;

NetworkingLayer::NetworkingLayer() {

}
NetworkingLayer::~NetworkingLayer() {

}
void NetworkingLayer::OnAttach() {
	if (enet_initialize()) {

	}
}
void NetworkingLayer::OnUpdate() {

}
void NetworkingLayer::OnDetach() {
	enet_deinitialize();
}