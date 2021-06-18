#include "EntityComponents.hpp"
#include <imgui.h>

void VSGEditor::DrawColliderComponent(VSGE::ColliderComponent* clc) {

	std::string shapes_str[3] = {
		"Box",
		"Sphere",
		"Mesh"
	};

	std::string current_shape = shapes_str[clc->GetShape()];

	if (ImGui::BeginCombo("Shape", current_shape.c_str())) {
		for (uint32 i = 0; i < 3; i ++) {
			bool is_selected = current_shape == shapes_str[i];
			if (ImGui::Selectable(shapes_str[i].c_str(), is_selected)) {
				current_shape = shapes_str[i].c_str();
				clc->SetColliderShape((VSGE::ColliderShape)i);
			}
		}

		ImGui::EndCombo();
	}

	Vec3 center = clc->GetCenter();
	Vec3 size = clc->GetSize();

	ImGui::InputFloat3("Center", &center.x);
	ImGui::InputFloat3("Size", &size.x);

	clc->SetCenter(center);
	clc->SetSize(size);
}