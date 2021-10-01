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

void VSGE::BindAudiosource() {
	mono_add_internal_call("AudioSourceComponent::i_Play(ulong)", Play);
	mono_add_internal_call("AudioSourceComponent::i_Pause(ulong)", Pause);
	mono_add_internal_call("AudioSourceComponent::i_Stop(ulong)", Stop);
}