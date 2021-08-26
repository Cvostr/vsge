#include "EntityComponents.hpp"
#include <imgui.h>
#include "../Misc/Thumbnails.hpp"
#include "../Windows/ResourcePickWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;

#define TERRAIN_TEXTURE_BTN_SIZE 48

float brush_size = 20;
float opacity = 15;
int edit_mode = TERRAIN_EDITOR_EDIT_MODE_HEIGHT;
uint32 picked_texture_index = 0;

void DrawTerrainTexturePickBtn(VSGE::ResourceReference& reference) {
	
	ImguiVulkanTexture* texture = TextureThumbnails::Get()->GetCheckerboardTexture();
	VSGE::Resource* resource = reference.GetResource();
	if (resource) {
		ImguiVulkanTexture* tex = TextureThumbnails::Get()->GetTextureResource(resource->GetDataDescription().file_path);
		if (tex)
			texture = tex;
	}
	if (ImGui::ImageButton((void*)texture->imtexture,
		ImVec2(TERRAIN_TEXTURE_BTN_SIZE, TERRAIN_TEXTURE_BTN_SIZE))
	) {
		ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
		rpw->SetAllowEmptyResource(true);
		rpw->SetResourceReference(&reference);
		rpw->Show();
	}
}

void DrawTerrainResourcePicker(uint32 texture_index, VSGE::TerrainTexture* terrain_texture) {
	bool is_picked = texture_index == picked_texture_index;
	std::string tte = "##" + std::to_string(texture_index);
	is_picked = ImGui::RadioButton(tte.c_str(), is_picked);
	if (is_picked)
		picked_texture_index = texture_index;
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_albedo_reference);
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_normal_reference);
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_roughness_reference);
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_metallic_reference);
}

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

	ImGui::Separator();

	if (ImGui::Button("H", ImVec2(32, 32)))
		edit_mode = TERRAIN_EDITOR_EDIT_MODE_HEIGHT;
	ImGui::SameLine();
	if(ImGui::Button("T", ImVec2(32, 32)))
		edit_mode = TERRAIN_EDITOR_EDIT_MODE_TEXTURES;
	ImGui::SameLine();
	if(ImGui::Button("G", ImVec2(32, 32)))
		edit_mode = TERRAIN_EDITOR_EDIT_MODE_GRASS;

	ImGui::Separator();

	if (edit_mode == TERRAIN_EDITOR_EDIT_MODE_TEXTURES) {
		uint32 textures_count = tc->GetTerrainTextures().size();

		for (uint32 i = 0; i < textures_count; i++) {
			VSGE::TerrainTexture* texture = &(tc->GetTerrainTextures()[i]);
			DrawTerrainResourcePicker(i, texture);
		}

		if (ImGui::Button("Add Texture", ImVec2(ImGui::GetWindowWidth(), 0))) {
			tc->AddNewTexture();
		}
	}
}

int VSGEditor::GetTerrainEditorMode() {
	return edit_mode;
}
float VSGEditor::GetTerrainEditorOpacity() {
	return opacity;
}
float VSGEditor::GetTerrainEditorBrushSize() {
	return brush_size;
}
uint32 VSGEditor::GetTerrainEditorTextureIndex() {
	return picked_texture_index;
}