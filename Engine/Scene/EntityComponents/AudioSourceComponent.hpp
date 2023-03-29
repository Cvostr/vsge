#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/AudioClipResource.hpp>
#include <Resources/ResourceReference.hpp>
#include <vector>

namespace VSGE {

	class AudioSourceComponent : public IEntityComponent {
	private:
		bool _playing;
		bool _playing_queued;
		bool _paused;
		uint32 _startTime;
		uint32 _clipDuration;

		ResourceReference _audioResource;

		bool _loop;
		float _volume;
		float _pitch;

		uint32 _audio_source;
		bool _created;
	public:
		AudioSourceComponent();
		/// <summary>
		/// Initialize audio source
		/// </summary>
		void Create();
		/// <summary>
		/// Destroy audio source
		/// </summary>
		void Destroy();

		bool IsPlaying();

		bool IsPaused();

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

		void SetAudioClip(AudioClipResource* resource);

		void OnPreRender();
		void OnDestroy();

		void OnActivate();
		void OnDeactivate();
		
		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_AUDIO_SOURCE, "Audio Source")
	};
}