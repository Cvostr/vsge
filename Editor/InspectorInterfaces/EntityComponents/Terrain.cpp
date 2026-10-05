#include "EntityComponents.hpp"
#include <imgui.h>
#include <Misc/Thumbnails.hpp>
#include <Windows/ResourcePickWindow.hpp>
#include <EditorLayers/ImGuiLayer.hpp>
#include <ImageBtnText.h>
#include <Base/ImguiHelper.hpp>
#include "../ResourcePicker.hpp"

using namespace VSGEditor;

#define TERRAIN_TEXTURE_BTN_SIZE 48

float brush_size = 20;
float opacity = 15;
int edit_mode = TERRAIN_EDITOR_EDIT_MODE_HEIGHT;
uint32 picked_texture_index = 0;
uint32 picked_vegetable_index = 0;

void DrawTerrainTexturePickBtn(VSGE::ResourceReference& reference, const std::string& tex_type, uint32 texture_index) 
{

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
		rpw->SetResourceType(VSGE::RESOURCE_TYPE_TEXTURE);
		rpw->SetResourceReference(&reference);
		rpw->Show();
	}
}
void DrawTerrainResourcePicker(uint32 texture_index, VSGE::TerrainTexture* terrain_texture) 
{
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
	DrawFloatControl(roughness_text, roughness_f);
	if (roughness_f <= 1.f)
		terrain_texture->_roughness_factor = roughness_f;
	else
		terrain_texture->_roughness_factor = 1.f;
	float metallic_f = terrain_texture->_metallic_factor;
	DrawFloatControl(metallic_text, metallic_f);
	if (metallic_f <= 1.f)
		terrain_texture->_metallic_factor = metallic_f;
	else
		terrain_texture->_metallic_factor = 1.f;
	DrawFloatControl(height_text, terrain_texture->_height_factor);
}

void DrawTerrainVegetableEditor(uint32 vegetable_index, VSGE::TerrainComponent* tc, VSGE::TerrainVegetable* vegetable)
{
	bool is_picked = vegetable_index == picked_vegetable_index;

	std::string tte = "##" + std::to_string(vegetable_index);
	is_picked = ImGui::RadioButton(tte.c_str(), is_picked);
	if (is_picked)
		picked_vegetable_index = vegetable_index;

	DrawResourcePicker(std::string("Mesh"), vegetable->_mesh_reference, VSGE::RESOURCE_TYPE_MESH);
	DrawResourcePicker(std::string("Material"), vegetable->_material_reference, VSGE::RESOURCE_TYPE_MATERIAL);

	std::string width_text = "Width" + tte;
	std::string height_text = "Height" + tte;

	float width_f = vegetable->m_width;
	float height_f = vegetable->m_height;

	if (DrawFloatControl(width_text, width_f)) {
		vegetable->SetWidth(width_f);
		tc->SetVegetablesDirty();
	}

	if (DrawFloatControl(height_text, height_f)) {
		vegetable->SetHeight(height_f);
		tc->SetVegetablesDirty();
	}
}

void VSGEditor::DrawTerrainComponent(VSGE::TerrainComponent* tc) 
{
	int width = tc->GetWidth();
	int length = tc->GetLength();

	DrawIntControl("Terrain Width", width);
	DrawIntControl("Terrain Length", length);
	
	tc->SetWidth(width);
	tc->SetLength(length);
	
	bool castShadows = tc->IsShadowCastEnabled();
	DrawCheckboxControl("Cast shadows", castShadows);
	tc->SetShadowCastEnabled(castShadows);

	ImGui::Separator();
	if (ImGui::Button("Clear terrain", ImVec2(ImGui::GetWindowWidth(), 0))) {
		tc->Flat(0);
	}
	ImGui::Separator();

	ImGui::Separator();
	DrawFloatControl("Brush size", brush_size);
	DrawFloatControl("Opacity", opacity);
	ImGui::Separator();

	if (ImGui::Button("H", ImVec2(32, 32)))
		edit_mode = TERRAIN_EDITOR_EDIT_MODE_HEIGHT;
	ImGui::SameLine();
	if (ImGui::Button("T", ImVec2(32, 32)))
		edit_mode = TERRAIN_EDITOR_EDIT_MODE_TEXTURES;
	ImGui::SameLine();
	if (ImGui::Button("G", ImVec2(32, 32)))
		edit_mode = TERRAIN_EDITOR_EDIT_MODE_VEGETABLES;
	ImGui::Separator();


	if (edit_mode == TERRAIN_EDITOR_EDIT_MODE_TEXTURES) 
	{
		Vec2i tileSize = tc->GetTexturesTileSize();
		DrawVec2iControl("Tile Size", tileSize);
		tc->SetTexturesTileSize(tileSize);

		size_t textures_count = tc->GetTerrainTextures().size();
		for (size_t i = 0; i < textures_count; i++) {
			VSGE::TerrainTexture* texture = &(tc->GetTerrainTextures()[i]);
			DrawTerrainResourcePicker(i, texture);
		}

		if (tc->GetTerrainTextures().size() < MAX_TEXTURES_PER_TERRAIN)
			if (ImGui::Button("Add Texture", ImVec2(ImGui::GetWindowWidth(), 0))) {
				tc->AddTexture();
		}
	}
	else if (edit_mode == TERRAIN_EDITOR_EDIT_MODE_VEGETABLES)
	{
		size_t vegetables_count = tc->GetTerrainVegetables().size();
		for (size_t i = 0; i < vegetables_count; i++) {
			VSGE::TerrainVegetable* veg = &(tc->GetTerrainVegetables()[i]);
			DrawTerrainVegetableEditor(i, tc, veg);
		}
		
		if (ImGui::Button("Add Vegetable", ImVec2(ImGui::GetWindowWidth(), 0))) {
			auto new_veg = VSGE::TerrainVegetable();
			new_veg.Clear(tc->GetWidth(), tc->GetLength());
			tc->GetTerrainVegetables().push_back(std::move(new_veg));
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
	return picked_vegetable_index;
}