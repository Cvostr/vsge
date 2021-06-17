#pragma once

#include "../Resource.hpp"
#include <Audio/AudioBuffer.hpp>

namespace VSGE {
	class AudioClipResource : public Resource {
	public:
		ResourceType GetResourceType() {
			return RESOURCE_TYPE_AUDIOCLIP;
		}
	};
}