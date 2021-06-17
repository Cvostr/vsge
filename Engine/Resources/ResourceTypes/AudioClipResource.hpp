#pragma once

#include "../Resource.hpp"
#include <Audio/AudioBuffer.hpp>

namespace VSGE {
	class AudioClipResource : public Resource {
	private:
		AudioBuffer* _buffer;
	public:
		AudioClipResource() {
			_buffer = new AudioBuffer;
		}

		AudioBuffer* GetAudioBuffer() { return _buffer; }

		void PostLoad() {
			_buffer->loadBufferWAV(_loadedData);
		}

		ResourceType GetResourceType() {
			return RESOURCE_TYPE_AUDIOCLIP;
		}
	};
}