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
	mono_add_internal_call("AudioSourceComponent::i_Play(ulong)", Play);
	mono_add_internal_call("AudioSourceComponent::i_Pause(ulong)", Pause);
	mono_add_internal_call("AudioSourceComponent::i_Stop(ulong)", Stop);
	mono_add_internal_call("AudioSourceComponent::i_IsPlaying(ulong)", IsPlaying);

	mono_add_internal_call("AudioSourceComponent::i_GetVolume(ulong)", GetVolume);
	mono_add_internal_call("AudioSourceComponent::i_SetVolume(ulong,float)", SetVolume);

	mono_add_internal_call("AudioSourceComponent::i_GetPitch(ulong)", GetPitch);
	mono_add_internal_call("AudioSourceComponent::i_SetPitch(ulong,float)", SetPitch);

	mono_add_internal_call("AudioSourceComponent::i_IsLooping(ulong)", IsLooping);
	mono_add_internal_call("AudioSourceComponent::i_SetLooping(ulong,bool)", SetLooping);

	mono_add_internal_call("AudioSourceComponent::i_SetAudioClip(ulong,ulong)", SetAudioClip);
}