#include "EntityComponents.hpp"
#include <imgui.h>
#include <Base/ImguiHelper.hpp>

using namespace VSGEditor;
using namespace VSGE;

void VSGEditor::DrawParticleEmitterComponent(VSGE::ParticleEmitterComponent* pec) {
	bool looping = pec->GetLooping();
	DrawCheckboxControl("Looping", looping);
	pec->SetLooping(looping);

	bool prewarm = pec->GetPrewarm();
	DrawCheckboxControl("Prewarm", prewarm);
	pec->SetPrewarm(prewarm);

	float duration = pec->GetDuration();
	DrawFloatControl("Duration", duration);
	pec->SetDuration(duration);

	int32 maxParticles = pec->GetMaxParticlesCount();
	DrawIntControl("Max Particles", maxParticles);
	pec->SetMaxParticlesCount(maxParticles);

	float particleLifetime = pec->GetParticleLifetime();
	DrawFloatControl("Particle Lifetime", particleLifetime);
	pec->SetParticleLifetime(particleLifetime);

	ImGui::Separator();

	MinMaxValue<int> emissionRate = pec->GetEmissionRate();
	ImGui::Text("Emission rate");
	DrawIntControl("Min##emrate", emissionRate.Min);
	DrawIntControl("Max##emrate", emissionRate.Max);
	pec->SetEmissionRate(emissionRate.Min, emissionRate.Max);

	ImGui::Separator();

	MinMaxValue<Vec3> direction = pec->GetDirection();
	ImGui::Text("Direction");
	DrawVec3Control("Min##direction", direction.Min);
	DrawVec3Control("Max##direction", direction.Max);
	pec->SetDirection(direction.Min, direction.Max);

	ImGui::Separator();

	MinMaxValue<float> velocity = pec->GetVelocity();
	ImGui::Text("Velocity");
	DrawFloatControl("Min##velocity", velocity.Min);
	DrawFloatControl("Max##velocity", velocity.Max);
	pec->SetVelocity(velocity.Min, velocity.Max);

	ImGui::Separator();

	Vec3 constantForce = pec->GetConstantForce();
	DrawVec3Control("Constant Force", constantForce);
	pec->SetConstantForce(constantForce);

	ImGui::Separator();

	float dampingForce = pec->GetDampingForce();
	DrawFloatControl("Damping Force", dampingForce);
	pec->SetDampingForce(dampingForce);

	ImGui::Separator();

	DeltaValue<MinMaxValue<Vec2>> size = pec->GetSize();
	ImGui::Text("Size");
	DrawVec2Control("Min##size", size.OriginalValue.Min);
	DrawVec2Control("Max##size", size.OriginalValue.Max);
	DrawFloatControl("Add", size.Add);
	DrawFloatControl("Multiply", size.Mul);
	pec->SetSize(size);

	ImGui::Separator();

	MinMaxValue<float> rotation = pec->GetRotation();
	ImGui::Text("Rotation");
	DrawFloatControl("Min##rotation", rotation.Min);
	DrawFloatControl("Max##rotation", rotation.Max);
	pec->SetRotation(rotation.Min, rotation.Max);

	ImGui::Separator();

	MinMaxValue<float> rotationSpeed = pec->GetRotationSpeed();
	ImGui::Text("Rotation speed");
	DrawFloatControl("Min##rotationspeed", rotationSpeed.Min);
	DrawFloatControl("Max##rotationspeed", rotationSpeed.Max);
	pec->SetRotationSpeed(rotationSpeed.Min, rotationSpeed.Max);

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