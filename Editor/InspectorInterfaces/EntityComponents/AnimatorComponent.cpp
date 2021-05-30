#include "EntityComponents.hpp"
#include <imgui.h>
#include "../ResourcePicker.hpp"

void VSGEditor::DrawAnimatorComponent(VSGE::AnimatorComponent* anic) {
	

	int i = 0;
	for (auto& anim : anic->GetAnimations()) {
		DrawResourcePicker(std::string("Animation " + std::to_string(i++)), anim._animResource);
		ImGui::InputFloat(std::string("Coeff").c_str(), &anim.coeff);
		ImGui::Separator();
	}

	if (ImGui::Button("Add animation")) {
		anic->NewAnimation();
	}
}