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

	if (!asc->IsPlaying()) {
		if (ImGui::Button("Play")) {
			asc->Play();
		}
	}
	else {
		if (ImGui::Button("Stop")) {
			asc->Stop();
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause")) {
			asc->Pause();
		}
	}
}