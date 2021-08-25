#include "EntityComponents.hpp"
#include <imgui.h>

using namespace VSGEditor;

void VSGEditor::DrawTerrainComponent(VSGE::TerrainComponent* tc) {
	int width = tc->GetWidth();
	int height = tc->GetHeight();

	ImGui::InputInt("Terrain Width", &width);
	ImGui::InputInt("Terrain Height", &height);

	tc->SetWidth(width);
	tc->SetHeight(height);

	if (ImGui::Button("TEST FLAT", ImVec2(ImGui::GetWindowWidth(), 0))) {
		tc->Flat(10);
		tc->UpdateMesh();
	}

	if (ImGui::Button("H1", ImVec2(ImGui::GetWindowWidth(), 0))) {
		tc->ModifyHeight(Vec2i(100, 100), 50, 20);
		tc->UpdateMesh();
	}
}