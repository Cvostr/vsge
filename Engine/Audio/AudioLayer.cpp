#include "AudioLayer.hpp"
#include <Core/Logger.hpp>
#include <AL/al.h>

using namespace VSGE;

AudioLayer* AudioLayer::_this = nullptr;

AudioLayer::AudioLayer() {
	_this = this;
	_created = false;
	_openal_context = nullptr;

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

	_created = true;

	//set base listener values
	SetListenerPos(Vec3(0, 0, 0));
	SetListenerOrientation(Vec3(0, 0, 1), Vec3(0, 1, 0));
	SetListenerVolume(1.f);

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
		_openal_context = nullptr;
	}
	if (_openal_device) {
		alcCloseDevice(_openal_device);
		_openal_device = nullptr;
	}

	_created = false;
}

void AudioLayer::SetListenerPos(const Vec3& pos) {
	if(_created)
		alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}
void AudioLayer::SetListenerOrientation(const Vec3& front, const Vec3& up) {
	if (_created) {
		ALfloat listenerOri[] = { front.x, front.y, front.z, up.x, up.y, up.z };
		alListenerfv(AL_ORIENTATION, listenerOri);
	}
}
void AudioLayer::SetListenerVolume(float volume) {
	if (_created) {
		alListenerf(AL_GAIN, volume);
	}
}