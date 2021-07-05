#include "EntityComponents.hpp"
#include <imgui.h>

using namespace VSGEditor;
using namespace VSGE;

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

	ImGui::Separator();

	MinMaxValue<int> emissionRate = pec->GetEmissionRate();
	ImGui::Text("Emission rate");
	ImGui::InputInt("Min", &emissionRate.Min);
	ImGui::InputInt("Max", &emissionRate.Max);
	pec->SetEmissionRate(emissionRate.Min, emissionRate.Max);

	ImGui::Separator();

	MinMaxValue<Vec3> direction = pec->GetDirection();
	ImGui::Text("Direction");
	ImGui::InputFloat3("Min", &direction.Min.x);
	ImGui::InputFloat3("Max", &direction.Max.x);
	pec->SetDirection(direction.Min, direction.Max);


	if (!pec->IsSimulating()) {
		if (ImGui::Button("Play", ImVec2(ImGui::GetWindowWidth(), 0))) {
			pec->StartSimulation();
		}
	}
	else {
		if (ImGui::Button("Stop", ImVec2(ImGui::GetWindowWidth(), 0))) {
			pec->StopSimulation();
		}
	}
}