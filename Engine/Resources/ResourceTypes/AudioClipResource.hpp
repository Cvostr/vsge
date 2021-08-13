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

		~AudioClipResource() {
			delete _buffer;
		}

		AudioBuffer* GetAudioBuffer() { return _buffer; }

		void OnRelease() {
			_buffer->Destroy();
		}

		void PostLoad() {
			_buffer->loadBufferWAV(_loadedData);
		}

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_AUDIOCLIP)
	};
}