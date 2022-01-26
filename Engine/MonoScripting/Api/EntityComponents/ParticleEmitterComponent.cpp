#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void StartSimulation(ParticleEmitterComponent* pec) {
	pec->StartSimulation();
}

static void StopSimulation(ParticleEmitterComponent* pec) {
	pec->StopSimulation();
}

static bool IsSimulating(ParticleEmitterComponent* pec) {
	return pec->IsSimulating();
}

static float GetDuration(ParticleEmitterComponent* pec) {
	return pec->GetDuration();
}

static void SetDuration(ParticleEmitterComponent* pec, float duration) {
	pec->SetDuration(duration);
}

static bool IsLooping(ParticleEmitterComponent* pec) {
	return pec->GetLooping();
}

static void SetLooping(ParticleEmitterComponent* pec, bool loop) {
	pec->SetLooping(loop);
}

static bool IsPrewarm(ParticleEmitterComponent* pec) {
	return pec->GetPrewarm();
}

static void SetPrewarm(ParticleEmitterComponent* pec, bool prewarm) {
	pec->SetPrewarm(prewarm);
}

static float GetParticleLifetime(ParticleEmitterComponent* pec) {
	return pec->GetParticleLifetime();
}

static void SetParticleLifetime(ParticleEmitterComponent* pec, float lifetime) {
	pec->SetParticleLifetime(lifetime);
}

void VSGE::BindParticleEmitter() {
	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_StartSimulation(ulong)", StartSimulation);
	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_StopSimulation(ulong)", StopSimulation);
	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_IsSimulating(ulong)", IsSimulating);

	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_GetDuration(ulong)", GetDuration);
	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_SetDuration(ulong,float)", SetDuration);

	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_IsLooping(ulong)", IsLooping);
	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_SetLooping(ulong,bool)", SetLooping);

	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_IsPrewarm(ulong)", IsPrewarm);
	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_SetPrewarm(ulong,bool)", SetPrewarm);

	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_GetParticleLifetime(ulong)", GetParticleLifetime);
	MonoScriptingLayer::AddInternalCall("ParticleEmitterComponent::i_SetParticleLifetime(ulong,float)", SetParticleLifetime);
}