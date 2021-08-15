#include "AudioClipResource.hpp"

using namespace VSGE;

AudioClipResource::AudioClipResource() {
	_buffer = new AudioBuffer;
}

AudioClipResource::~AudioClipResource() {
	delete _buffer;
}

AudioBuffer* AudioClipResource::GetAudioBuffer() { return _buffer; }

void AudioClipResource::OnRelease() {
	_buffer->Destroy();
}

void AudioClipResource::PostLoad() {
	_buffer->loadBufferWAV(_loadedData);
}