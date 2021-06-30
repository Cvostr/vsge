#include "EntityComponents.hpp"
#include <imgui.h>
#include "../ResourcePicker.hpp"

using namespace VSGEditor;

void VSGEditor::DrawAudioSourceComponent(VSGE::AudioSourceComponent* asc) {
	DrawResourcePicker(std::string("Audio Clip"), asc->GetResourceReference());

	float volume = asc->GetVolume();
	ImGui::InputFloat("Volume", &volume);
	asc->SetVolume(volume);

	float pitch = asc->GetPitch();
	ImGui::InputFloat("Pitch", &pitch);
	asc->SetPitch(pitch);

	bool loop = asc->IsLoop();
	ImGui::Checkbox("Loop", &loop);
	asc->SetLoop(loop);

	if (!asc->IsActive())
		return;

	if (asc->IsPaused()) {
		if (ImGui::Button("Resume", ImVec2(ImGui::GetWindowWidth(), 0))) {
			asc->Play();
		}
	}
	if (!asc->IsPlaying() && !asc->IsPaused()) {
		if (ImGui::Button("Play", ImVec2(ImGui::GetWindowWidth(), 0))) {
			asc->Play();
		}
	}
	else if(!asc->IsPaused()) {
		if (ImGui::Button("Stop", ImVec2(ImGui::GetWindowWidth() / 2, 0))) {
			asc->Stop();
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause", ImVec2(ImGui::GetWindowWidth() / 2, 0))) {
			asc->Pause();
		}
	}
}