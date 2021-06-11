#include "AudioSourceComponent.hpp"

using namespace VSGE;

bool AudioSourceComponent::IsPlaying() {
	return _playing;
}

float AudioSourceComponent::GetPitch() {
	return _pitch;
}

void AudioSourceComponent::SetPitch(float pitch) {
	_pitch = pitch;
}

void AudioSourceComponent::Play() {

}

void AudioSourceComponent::Pause() {

}

void AudioSourceComponent::Stop() {

}