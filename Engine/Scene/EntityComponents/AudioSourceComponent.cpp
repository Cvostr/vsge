#include "AudioSourceComponent.hpp"
#include "Core/Time.hpp"

using namespace VSGE;
using namespace YAML;

AudioSourceComponent::AudioSourceComponent() :
	_volume(1.f),
	_pitch(1.f),
	_loop(false),

	_playing(false),
	_startTime(0)
{
	_audioResource.SetResourceType(RESOURCE_TYPE_AUDIOCLIP);
}

bool AudioSourceComponent::IsLoop() {
	return _loop;
}

void AudioSourceComponent::SetLoop(bool loop) {
	_loop = loop;
}

bool AudioSourceComponent::IsPlaying() {
	return _playing;
}

float AudioSourceComponent::GetPitch() {
	return _pitch;
}

void AudioSourceComponent::SetPitch(float pitch) {
	_pitch = pitch;
}

float AudioSourceComponent::GetVolume() {
	return _volume;
}

void AudioSourceComponent::SetVolume(float volume) {
	_volume = volume;
}

ResourceReference& AudioSourceComponent::GetResourceReference() {
	return _audioResource;
}

void AudioSourceComponent::Play() {
	if (_playing)
		return;

	_startTime = (uint32)TimePerf::Get()->GetCurrentTime();

	_playing = true;
}

void AudioSourceComponent::Pause() {

}

void AudioSourceComponent::Stop() {
	if (!_playing)
		return;

	_playing = false;
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