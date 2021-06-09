#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/AnimationResource.hpp>
#include <vector>

namespace VSGE {

	class AudioSourceComponent : public IEntityComponent {
	private:
		bool _playing;
		double _startTime;

		ResourceReference _audioResource;

		bool _loop;
		float _volume;

	public:
		AudioSourceComponent() {
			_audioResource.SetResourceType(RESOURCE_TYPE_AUDIOCLIP);
		}

		void Play();

		void Pause();

		void Stop();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_AUDIO_SOURCE, "Audio Source")
	};
}