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
	ImGui::InputInt("Min##emrate", &emissionRate.Min);
	ImGui::InputInt("Max##emrate", &emissionRate.Max);
	pec->SetEmissionRate(emissionRate.Min, emissionRate.Max);

	ImGui::Separator();

	MinMaxValue<Vec3> direction = pec->GetDirection();
	ImGui::Text("Direction");
	ImGui::InputFloat3("Min##direction", &direction.Min.x);
	ImGui::InputFloat3("Max##direction", &direction.Max.x);
	pec->SetDirection(direction.Min, direction.Max);

	ImGui::Separator();

	MinMaxValue<float> velocity = pec->GetVelocity();
	ImGui::Text("Velocity");
	ImGui::InputFloat("Min##velocity", &velocity.Min);
	ImGui::InputFloat("Max##velocity", &velocity.Max);
	pec->SetVelocity(velocity.Min, velocity.Max);

	ImGui::Separator();

	Vec3 constantForce = pec->GetConstantForce();
	ImGui::InputFloat3("Constant Force", &constantForce.x);
	pec->SetConstantForce(constantForce);

	ImGui::Separator();

	float dampingForce = pec->GetDampingForce();
	ImGui::InputFloat("Damping Force", &dampingForce);
	pec->SetDampingForce(dampingForce);

	ImGui::Separator();

	DeltaValue<MinMaxValue<Vec2>> size = pec->GetSize();
	ImGui::Text("Size");
	ImGui::InputFloat2("Min##size", &size.OriginalValue.Min.x);
	ImGui::InputFloat2("Max##size", &size.OriginalValue.Max.x);
	ImGui::InputFloat("Add", &size.Add);
	ImGui::InputFloat("Multiply", &size.Mul);
	pec->SetSize(size);

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