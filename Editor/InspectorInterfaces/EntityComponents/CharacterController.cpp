#include "EntityComponents.hpp"
#include <imgui.h>

using namespace VSGE;
using namespace VSGEditor;

void VSGEditor::DrawCharControllerComponent(VSGE::CharacterControllerComponent* ccc) {
	Vec3 center = ccc->GetCenter();
	ImGui::InputFloat3("Center", &center.x);
	ccc->SetCenter(center);

	Vec2 size = ccc->GetSize();
	ImGui::InputFloat("Radius", &size.x);
	ImGui::InputFloat("Height", &size.y);
	ccc->SetSize(size);
}