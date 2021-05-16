#pragma once

#include "Serializer.hpp"
#include "DeSerializer.hpp"

namespace VSGE {
	class ISerializable {
	public:

		virtual void Serialize(Serializer& s) {}
		virtual void DeSerialize(Deserializer& ds) {}

	};
}