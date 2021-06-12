#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/AnimationResource.hpp>
#include <vector>

namespace VSGE {

	class AudioSourceComponent : public IEntityComponent {
	private:
		bool _playing;
		uint32 _startTime;

		ResourceReference _audioResource;

		bool _loop;
		float _volume;
		float _pitch;
	public:
		AudioSourceComponent();

		bool IsPlaying();

		bool IsLoop();

		void SetLoop(bool loop);

		float GetPitch();
		
		void SetPitch(float pitch);

		float GetVolume();

		void SetVolume(float volume);

		ResourceReference& GetResourceReference();

		void Play();

		void Pause();

		void Stop();

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_AUDIO_SOURCE, "Audio Source")
	};
}