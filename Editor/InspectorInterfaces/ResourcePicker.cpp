#include "ResourcePicker.hpp"
#include <Resources/ResourceCache.hpp>
#include "../Windows/ResourcePickWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../Misc/Thumbnails.hpp"
#include "../Misc/VkMaterialsThumbnails.hpp"
#include "ImageBtnText.h"

using namespace VSGE;
using namespace VSGEditor;

void VSGEditor::DrawResourcePicker(std::string label, ResourceReference& reference, bool empty_res, uint32 index)
{
	ImVec2 win_size = ImGui::GetWindowSize();
	ImVec2 text_size = ImGui::CalcTextSize(label.c_str());

	ImVec2 cursor = ImGui::GetCursorPos();
	if (reference.GetResourceType() == RESOURCE_TYPE_TEXTURE || reference.GetResourceType() == RESOURCE_TYPE_MATERIAL)
		ImGui::SetCursorPos(cursor + ImVec2(0, 32));

	ImGui::Text(label.c_str());

	ImGui::SameLine();

	ImguiVulkanTexture* texture = nullptr;
	ImTextureID mat_texture = nullptr;

	if (reference.GetResourceType() == RESOURCE_TYPE_TEXTURE) {
		texture = TextureThumbnails::Get()->GetCheckerboardTexture();

		Resource* resource = reference.GetResource();
		
		if (resource) {
			ImguiVulkanTexture* tex = TextureThumbnails::Get()->GetTextureResource(resource->GetDataDescription().file_path);
			if (tex)
				texture = tex;
		}
	}
	else if (reference.GetResourceType() == RESOURCE_TYPE_MATERIAL) {
		texture = TextureThumbnails::Get()->GetCheckerboardTexture();

		Resource* resource = reference.GetResource();

		if (resource) {
			if (!resource->IsReady()) {
				resource->Load();
			}
		
			ImTextureID thumb = VkMaterialsThumbnails::Get()->GetMaterialThumbnailTexture(resource->GetName());
			if (resource->IsReady())
				VkMaterialsThumbnails::Get()->CreateThumbnail(resource->GetName());
			if (thumb)
				mat_texture = thumb;
		}
	}
	else {
		ImGui::Text(reference.GetResourceName().c_str());
		text_size.x += ImGui::CalcTextSize(reference.GetResourceName().c_str()).x;
		
	}

	ImGui::SameLine();

	cursor = ImGui::GetCursorPos();

	if (reference.GetResourceType() == RESOURCE_TYPE_TEXTURE || reference.GetResourceType() == RESOURCE_TYPE_MATERIAL)
		ImGui::SetCursorPos(cursor + ImVec2(win_size.x - text_size.x - 120, -32));
	else
		ImGui::SetCursorPos(cursor + ImVec2(win_size.x - text_size.x - 90, 0));

	std::string btn_text = "Select##" + label + std::to_string(index);
	if (reference.GetResourceType() == RESOURCE_TYPE_TEXTURE && texture) {
		if (ImageButtonWithText((void*)texture->imtexture, btn_text.c_str(), nullptr, nullptr, ImVec2(64, 64))) {
			ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
			rpw->SetAllowEmptyResource(empty_res);
			rpw->SetResourceReference(&reference);
			rpw->Show();
		}
	}
	else if (reference.GetResourceType() == RESOURCE_TYPE_MATERIAL && mat_texture) {
		if (ImageButtonWithText((void*)mat_texture, btn_text.c_str(), nullptr, nullptr, ImVec2(64, 64))) {
			ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
			rpw->SetAllowEmptyResource(empty_res);
			rpw->SetResourceReference(&reference);
			rpw->Show();
		}
	}
	else if (ImGui::Button(btn_text.c_str())) {
		ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
		rpw->SetAllowEmptyResource(empty_res);
		rpw->SetResourceReference(&reference);
		rpw->Show();
	}
	ImGui::Separator();
}