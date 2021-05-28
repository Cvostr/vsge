#include "EntityComponents.hpp"
#include <imgui.h>

using namespace VSGEditor;

void VSGEditor::DrawParticleEmitterComponent(VSGE::ParticleEmitterComponent* pec) {
	bool looping = pec->GetLooping();
	ImGui::Checkbox("Looping", &looping);
	pec->SetLooping(looping);

	bool prewarm = pec->GetPrewarm();
	ImGui::Checkbox("Prewarm", &prewarm);
	pec->SetPrewarm(prewarm);

	float duration = pec->GetDuration();
	ImGui::InputFloat("Duration", &duration);
	pec->SetDuration(duration);

	uint32 maxParticles = pec->GetMaxParticlesCount();
	ImGui::InputInt("Max Particles", (int*)&maxParticles);
	pec->SetMaxParticlesCount(maxParticles);

	float particleLifetime = pec->GetParticleLifetime();
	ImGui::InputFloat("Particle Lifetime", &particleLifetime);
	pec->SetParticleLifetime(particleLifetime);
}