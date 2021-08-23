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
}