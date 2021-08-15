#pragma once

#include "../Resource.hpp"
#include <Audio/AudioBuffer.hpp>

namespace VSGE {
	class AudioClipResource : public Resource {
	private:
		AudioBuffer* _buffer;
	public:
		AudioClipResource();

		~AudioClipResource();

		AudioBuffer* GetAudioBuffer();

		void OnRelease();

		void PostLoad();

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_AUDIOCLIP)
	};
}