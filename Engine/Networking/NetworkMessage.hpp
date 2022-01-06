#pragma once

#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	class NetworkMessage {
	private:
		byte* _buffer;
		uint32 _buffer_size;

		uint32 _id;
	public:

		NetworkMessage();
		~NetworkMessage();

	};
}