#include "EntityComponents.hpp"
#include <imgui.h>
#include "../Misc/Thumbnails.hpp"
#include "../Windows/ResourcePickWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include <ImageBtnText.h>
#include "../ResourcePicker.hpp"

using namespace VSGEditor;

#define TERRAIN_TEXTURE_BTN_SIZE 48

float brush_size = 20;
float opacity = 15;
int edit_mode = TERRAIN_EDITOR_EDIT_MODE_HEIGHT;
uint32 picked_texture_index = 0;
uint32 picked_grass_index = 0;

void DrawTerrainTexturePickBtn(VSGE::ResourceReference& reference, const std::string& tex_type,
	 uint32 texture_index) {
	
	ImguiVulkanTexture* texture = TextureThumbnails::Get()->GetCheckerboardTexture();
	VSGE::Resource* resource = reference.GetResource();
	if (resource) {
		ImguiVulkanTexture* tex = TextureThumbnails::Get()->GetTextureResource(resource->GetDataDescription().file_path);
		if (tex)
			texture = tex;
	}
	std::string text = tex_type + "##" + std::to_string(texture_index);
	if (ImageButtonWithText((void*)texture->imtexture, text.c_str(), nullptr, nullptr,
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

	DrawTerrainTexturePickBtn(terrain_texture->_albedo_reference, "Albedo", texture_index);
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_normal_reference, "Normal", texture_index);
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_roughness_reference, "Roughness", texture_index);
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_metallic_reference, "Metallic", texture_index);
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_ao_reference, "AO", texture_index);
	ImGui::SameLine();

	DrawTerrainTexturePickBtn(terrain_texture->_height_reference, "Height", texture_index);

	std::string roughness_text = "Roughness factor" + tte;
	std::string metallic_text = "Metallic factor" + tte;
	std::string height_text = "Height factor" + tte;

	float roughness_f = terrain_texture->_roughness_factor;
	ImGui::InputFloat(roughness_text.c_str(), &roughness_f);
	if (roughness_f <= 1.f)
		terrain_texture->_roughness_factor = roughness_f;
	else
		terrain_texture->_roughness_factor = 1.f;

	float metallic_f = terrain_texture->_metallic_factor;
	ImGui::InputFloat(metallic_text.c_str(), &metallic_f);
	if (metallic_f <= 1.f)
		terrain_texture->_metallic_factor = roughness_f;
	else
		terrain_texture->_metallic_factor = 1.f;

	ImGui::InputFloat(height_text.c_str(), &terrain_texture->_height_factor);
}

bool DrawTerrainGrassEdit(uint32 grass_index, VSGE::TerrainGrass* terrain_grass) {
	bool changed = false;
	bool is_picked = grass_index == picked_grass_index;
	std::string tte = "##" + std::to_string(grass_index);
	is_picked = ImGui::RadioButton(tte.c_str(), is_picked);
	if (is_picked)
		picked_grass_index = grass_index;

	DrawResourcePicker("Diffuse", terrain_grass->_texture_reference, true, grass_index);

	std::string width_text = "Grass width##" + std::to_string(grass_index);
	std::string height_text = "Grass height##" + std::to_string(grass_index);

	float width = terrain_grass->_width;
	float height = terrain_grass->_height;
	ImGui::InputFloat(width_text.c_str(), &width);
	ImGui::InputFloat(height_text.c_str(), &height);
	if (width != terrain_grass->_width || height != terrain_grass->_height)
		changed = true;
	terrain_grass->_width = width;
	terrain_grass->_height = height;

	return changed;
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
		uint32 textures_count = static_cast<uint32>(tc->GetTerrainTextures().size());

		for (uint32 i = 0; i < textures_count; i++) {
			VSGE::TerrainTexture* texture = &(tc->GetTerrainTextures()[i]);
			DrawTerrainResourcePicker(i, texture);
		}

		if(tc->GetTerrainTextures().size() < MAX_TEXTURES_PER_TERRAIN)
			if (ImGui::Button("Add Texture", ImVec2(ImGui::GetWindowWidth(), 0))) {
				tc->AddNewTexture();
			}
	}

	if (edit_mode == TERRAIN_EDITOR_EDIT_MODE_GRASS) {
		uint32 vegetables_count = static_cast<uint32>(tc->GetTerrainVegetables().size());

		for (uint32 i = 0; i < vegetables_count; i++) {
			VSGE::TerrainGrass* grass = &(tc->GetTerrainVegetables()[i]);
			if (DrawTerrainGrassEdit(i, grass))
				tc->UpdateVegetables();
		}

		if (ImGui::Button("Add Grass", ImVec2(ImGui::GetWindowWidth(), 0))) {
			tc->AddNewGrass();
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
uint32 VSGEditor::GetTerrainEditorVegetableIndex() {
	return picked_grass_index;
}