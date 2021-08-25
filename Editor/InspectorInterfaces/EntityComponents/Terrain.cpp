#include "EntityComponents.hpp"
#include <imgui.h>

using namespace VSGEditor;

float brush_size = 20;
float opacity = 15;

void VSGEditor::DrawTerrainComponent(VSGE::TerrainComponent* tc) {
	int width = tc->GetWidth();
	int height = tc->GetHeight();

	ImGui::InputInt("Terrain Width", &width);
	ImGui::InputInt("Terrain Height", &height);

	tc->SetWidth(width);
	tc->SetHeight(height);

	ImGui::Separator();
	if (ImGui::Button("Clear terrain", ImVec2(ImGui::GetWindowWidth(), 0))) {
		tc->Flat(0);
		tc->UpdateMesh();
		tc->UpdateTextureMasks();
	}

	ImGui::Separator();

	ImGui::InputFloat("Brush size", &brush_size);
	ImGui::InputFloat("Opacity", &opacity);
}

float VSGEditor::GetTerrainEditorOpacity() {
	return opacity;
}
float VSGEditor::GetTerrainEditorBrushSize() {
	return brush_size;
}