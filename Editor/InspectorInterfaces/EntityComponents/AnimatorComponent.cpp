#include "EntityComponents.hpp"
#include <imgui.h>
#include "../ResourcePicker.hpp"

void VSGEditor::DrawAnimatorComponent(VSGE::AnimatorComponent* anic) {
	

	int i = 0;
	for (auto& anim : anic->GetAnimations()) {
		DrawResourcePicker(std::string("Animation"), anim._animResource, true, i++);
		ImGui::InputFloat(std::string("Coeff##" + std::to_string(i)).c_str(), &anim.coeff);
		ImGui::Separator();
	}

	if (ImGui::Button("Add animation")) {
		anic->AddNewAnimation();
	}
	if (!anic->IsPlaying()) {
		if (ImGui::Button("Play", ImVec2(ImGui::GetWindowWidth(), 0))) {
			anic->Play();
		}
	}
	else {
		if (ImGui::Button("Stop", ImVec2(ImGui::GetWindowWidth() / 2, 0))) {
			anic->Stop();
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause", ImVec2(ImGui::GetWindowWidth() / 2, 0))) {
			anic->Pause();
		}
	}
}