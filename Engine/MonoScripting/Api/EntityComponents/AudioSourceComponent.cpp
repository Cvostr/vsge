#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/AudioSourceComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void Play(AudioSourceComponent* ptr) {
	ptr->Play();
}

static void Pause(AudioSourceComponent* ptr) {
	ptr->Pause();
}

static void Stop(AudioSourceComponent* ptr) {
	ptr->Stop();
}

static bool IsPlaying(AudioSourceComponent* ptr) {
	return ptr->IsPlaying();
}

static float GetVolume(AudioSourceComponent* ptr) {
	return ptr->GetVolume();
}
static void SetVolume(AudioSourceComponent* ptr, float volume) {
	ptr->SetVolume(volume);
}

static float GetPitch(AudioSourceComponent* ptr) {
	return ptr->GetVolume();
}
static void SetPitch(AudioSourceComponent* ptr, float pitch) {
	ptr->SetPitch(pitch);
}

static bool IsLooping(AudioSourceComponent* ptr) {
	return ptr->IsLoop();
}
static void SetLooping(AudioSourceComponent* ptr, bool loop) {
	ptr->SetLoop(loop);
}

static void SetAudioClip(AudioSourceComponent* ptr, Resource* res) {
	ptr->SetAudioClip((AudioClipResource*)res);
}

void VSGE::BindAudiosource() {
	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_Play(ulong)", Play);
	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_Pause(ulong)", Pause);
	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_Stop(ulong)", Stop);
	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_IsPlaying(ulong)", IsPlaying);

	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_GetVolume(ulong)", GetVolume);
	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_SetVolume(ulong,float)", SetVolume);

	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_GetPitch(ulong)", GetPitch);
	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_SetPitch(ulong,float)", SetPitch);

	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_IsLooping(ulong)", IsLooping);
	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_SetLooping(ulong,bool)", SetLooping);

	MonoScriptingLayer::AddInternalCall("AudioSourceComponent::i_SetAudioClip(ulong,ulong)", SetAudioClip);
}