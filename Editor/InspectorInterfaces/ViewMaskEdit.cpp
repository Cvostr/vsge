#include "ViewMaskEdit.hpp"
#include <imgui.h>

using namespace VSGE;

void VSGEditor::DrawViewMaskEditor(ViewMask& mask) {
	for (uint64 vm_i = 0; vm_i < 32; vm_i++) {
		bool vm = (mask >> vm_i) & 0x1;

		if (!vm)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(120, 120, 120, 0));

		std::string btn_id = "##vmb" + std::to_string(vm_i);
		if (ImGui::Button(btn_id.c_str(), ImVec2(10, 10))) {
			if (vm) {
				ViewMask submask = 0;
				for (uint64 sb = 0; sb < 32; sb++) {
					if (sb != vm_i)
						submask |= ((uint64)1 << sb);
				}
				mask &= submask;
			}
			else {
				mask |= ((uint64)1 << vm_i);
			}
			//mShowingEntity->SetViewMask(current_view_mask);
		}
		if (!vm)
			ImGui::PopStyleColor();
		if ((vm_i + 1) % 16 > 0)
			ImGui::SameLine();
	}
}