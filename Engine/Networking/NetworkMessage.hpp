#pragma once

#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	void Enet_SendPacket(void* peer, byte* data, uint32 size);
}