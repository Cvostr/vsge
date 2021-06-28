#include "AudioSourceComponent.hpp"
#include <Resources/ResourceTypes/AudioClipResource.hpp>
#include "Core/Time.hpp"
#include <AL/al.h>

using namespace VSGE;
using namespace YAML;

AudioSourceComponent::AudioSourceComponent() :
	_audio_source(0),
	_volume(1.f),
	_pitch(1.f),
	_loop(false),

	_playing(false),
	_playing_queued(false),
	_paused(false),

	_startTime(0),
	_clipDuration(0),
	_created(false)
{
	_audioResource.SetResourceType(RESOURCE_TYPE_AUDIOCLIP);
}

void AudioSourceComponent::Create() {
	if (!_created) {
		alGenSources(1, &_audio_source);
		_created = true;

		SetLoop(_loop);
		SetPitch(_pitch);
		SetVolume(_volume);
	}
}
void AudioSourceComponent::Destroy() {
	if (_created) {
		alDeleteSources(1, &_audio_source);
		_created = false;
	}
}

bool AudioSourceComponent::IsLoop() {
	return _loop;
}

void AudioSourceComponent::SetLoop(bool loop) {
	_loop = loop;

	if (_created) {
		alSourcei(_audio_source, AL_LOOPING, (int)loop);
	}
}

bool AudioSourceComponent::IsPlaying() {
	return _playing;
}

bool AudioSourceComponent::IsPaused() {
	return _paused;
}

float AudioSourceComponent::GetPitch() {
	return _pitch;
}

void AudioSourceComponent::SetPitch(float pitch) {
	_pitch = pitch;

	if (_created) {
		alSourcef(_audio_source, AL_PITCH, pitch);
	}
}

float AudioSourceComponent::GetVolume() {
	return _volume;
}

void AudioSourceComponent::SetVolume(float volume) {
	_volume = volume;

	if (_created) {
		alSourcef(_audio_source, AL_GAIN, volume);
	}
}

ResourceReference& AudioSourceComponent::GetResourceReference() {
	return _audioResource;
}

void AudioSourceComponent::Play() {
	if (_playing )
		return;

	if (!_created)
		Create();

	AudioClipResource* resource = (AudioClipResource*)_audioResource.GetResource();
	if (resource) {
		if (resource->IsUnloaded()) {
			_playing_queued = true;
			resource->Load();
		}
		else if (resource->IsReady()) {
			AudioBuffer* buffer = resource->GetAudioBuffer();
			_clipDuration = buffer->GetDuration();
			alSourcei(_audio_source, AL_BUFFER, static_cast<ALint>(buffer->GetBuffer()));
			alSourcePlay(_audio_source);

			_startTime = (uint32)TimePerf::Get()->GetCurrentTime();
			_playing = true;
			_paused = false;
			_playing_queued = false;
		}
	}
}

void AudioSourceComponent::Pause() {
	if (_created && _playing) {
		alSourcePause(_audio_source);
		_paused = true;
		_playing = false;

		uint32 current_time = (uint32)TimePerf::Get()->GetCurrentTime();
		uint32 delta = current_time - _startTime;

		_clipDuration -= delta;
	}
}

void AudioSourceComponent::Stop() {
	if (!_playing && !_paused)
		return;

	alSourceStop(_audio_source);

	_playing = false;
	_paused = false;
}

void AudioSourceComponent::OnPreRender() {
	if (_playing_queued) {
		Play();
	}

	if (_playing) {
		AudioClipResource* resource = (AudioClipResource*)_audioResource.GetResource();
		resource->Use();

		uint32 current_time = (uint32)TimePerf::Get()->GetCurrentTime();
		uint32 delta = current_time - _startTime;

		if (delta >= _clipDuration && !_loop) {
			Stop();
		}
	}
}

void AudioSourceComponent::OnDestroy() {
	Stop();
}

void AudioSourceComponent::Serialize(YAML::Emitter& e) {
	e << Key << "clip" << Value << _audioResource.GetResourceName();
	e << Key << "volume" << Value << _volume;
	e << Key << "pitch" << Value << _pitch;
	e << Key << "loop" << Value << _loop;
}
void AudioSourceComponent::Deserialize(YAML::Node& entity) {
	_audioResource.SetResource(entity["clip"].as<std::string>());
	_volume = entity["volume"].as<float>();
	_pitch = entity["pitch"].as<float>();
	_loop = entity["loop"].as<bool>();
}