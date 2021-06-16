#include "AudioLayer.hpp"
#include <Core/Logger.hpp>
#include <AL/al.h>

using namespace VSGE;

AudioLayer::AudioLayer() {
	_openal_device = alcOpenDevice(nullptr);

	if (!_openal_device) {
		Logger::Log(LogType::LOG_TYPE_ERROR) << "OpenAL: Can't initialize OpenAL device\n";
		return;
	}

	_openal_context = alcCreateContext(_openal_device, nullptr);

	if (!_openal_context) {
		Logger::Log(LogType::LOG_TYPE_ERROR) << "OpenAL: Can't initialize OpenAL device context\n";
		return;
	}

	alcMakeContextCurrent(_openal_context);

	if (alGetError() != AL_NO_ERROR)
	{
		Logger::Log(LogType::LOG_TYPE_ERROR) << "Can't set current context\n";
		return;
	}

	Logger::Log(LogType::LOG_TYPE_INFO) << "Audio initialized\n";
}

void AudioLayer::OnAttach() {

}
void AudioLayer::OnUpdate() {

}
void AudioLayer::OnDetach() {
	alcMakeContextCurrent(nullptr);
	if (_openal_context) {
		alcDestroyContext(_openal_context);
		_openal_context = false;
	}
	if (_openal_device) {
		alcCloseDevice(_openal_device);
		_openal_device = nullptr;
	}
}