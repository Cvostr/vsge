#include "NetworkingLayer.hpp"
#include <Core/Logger.hpp>

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
	if (enet_initialize() != 0) {
		Logger::Log(LogType::LOG_TYPE_ERROR) << "Enet Networking initialization failed!\n";
	}
	Logger::Log(LogType::LOG_TYPE_INFO) << "Enet Networking initialized\n";
}
void NetworkingLayer::OnUpdate() {

}
void NetworkingLayer::OnDetach() {
	enet_deinitialize();
	Logger::Log(LogType::LOG_TYPE_INFO) << "Enet Networking closed\n";
}