#include "EntityComponents.hpp"
#include <Base/ImguiHelper.hpp>
#include <imgui.h>

using namespace VSGE;
using namespace VSGEditor;

void VSGEditor::DrawCharControllerComponent(VSGE::CharacterControllerComponent* ccc) {
	Vec3 center = ccc->GetCenter();
	if (DrawVec3Control("Center", center))
		ccc->SetCenter(center);

	Vec2 size = ccc->GetSize();
	DrawFloatControl("Radius", size.x);
	DrawFloatControl("Height", size.y);
	ccc->SetSize(size);

	ImGui::Separator();

	float skinWidth = ccc->GetSkinWidth();
	if (DrawFloatControl("Skin Width", skinWidth))
		ccc->SetSkinWidth(skinWidth);
}