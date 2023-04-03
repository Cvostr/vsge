#include "ResourcePicker.hpp"
#include <Resources/ResourceCache.hpp>
#include "../Windows/ResourcePickWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../Misc/Thumbnails.hpp"
#include "../Misc/VkMaterialsThumbnails.hpp"
#include "ImageBtnText.h"

using namespace VSGE;
using namespace VSGEditor;

void VSGEditor::DrawResourcePicker(const std::string& label, ResourceReference& reference, ResourceType resourceType, bool empty_res, uint32 index)
{
	ImVec2 win_size = ImGui::GetWindowSize();
	ImVec2 text_size = ImGui::CalcTextSize(label.c_str());

	ImVec2 cursor = ImGui::GetCursorPos();
	if (resourceType == RESOURCE_TYPE_TEXTURE || resourceType == RESOURCE_TYPE_MATERIAL)
		ImGui::SetCursorPos(cursor + ImVec2(0, 32));

	ImGui::Text(label.c_str());

	ImGui::SameLine();

	ImguiVulkanTexture* texture = nullptr;
	ImTextureID mat_texture = nullptr;

	if (resourceType == RESOURCE_TYPE_TEXTURE) {
		texture = TextureThumbnails::Get()->GetCheckerboardTexture();

		Resource* resource = reference.GetResource();
		
		if (resource) {
			ImguiVulkanTexture* tex = TextureThumbnails::Get()->GetTextureResource(resource->GetDataDescription().file_path);
			if (tex)
				texture = tex;
		}
	}
	else if (resourceType == RESOURCE_TYPE_MATERIAL) {
		texture = TextureThumbnails::Get()->GetCheckerboardTexture();

		Resource* resource = reference.GetResource();

		if (resource) {
			if (!resource->IsReady()) {
				resource->Load();
			}
		
			ImTextureID thumb = VkMaterialsThumbnails::Get()->GetMaterialThumbnailTexture(resource->getId());
			if (resource->IsReady())
				VkMaterialsThumbnails::Get()->CreateThumbnail(resource);
			if (thumb)
				mat_texture = thumb;
		}
	}
	else {
		std::string resourceName = reference.GetResourceName();
		ImGui::Text(resourceName.c_str());
		text_size.x += ImGui::CalcTextSize(resourceName.c_str()).x;
		
	}

	ImGui::SameLine();

	cursor = ImGui::GetCursorPos();

	if (resourceType == RESOURCE_TYPE_TEXTURE || resourceType == RESOURCE_TYPE_MATERIAL)
		ImGui::SetCursorPos(cursor + ImVec2(win_size.x - text_size.x - 120, -32));
	else
		ImGui::SetCursorPos(cursor + ImVec2(win_size.x - text_size.x - 90, 0));

	std::string btn_text = "Select##" + label + std::to_string(index);
	if (resourceType == RESOURCE_TYPE_TEXTURE && texture) {
		if (ImageButtonWithText((void*)texture->imtexture, btn_text.c_str(), nullptr, nullptr, ImVec2(64, 64))) {
			ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
			rpw->SetAllowEmptyResource(empty_res);
			rpw->SetResourceType(resourceType);
			rpw->SetResourceReference(&reference);
			rpw->Show();
		}
	}
	else if (resourceType == RESOURCE_TYPE_MATERIAL) {
		if (ImageButtonWithText((void*)mat_texture, btn_text.c_str(), nullptr, nullptr, ImVec2(64, 64))) {
			ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
			rpw->SetAllowEmptyResource(empty_res);
			rpw->SetResourceType(resourceType);
			rpw->SetResourceReference(&reference);
			rpw->Show();
		}
	}
	else if (ImGui::Button(btn_text.c_str())) {
		ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
		rpw->SetAllowEmptyResource(empty_res);
		rpw->SetResourceType(resourceType);
		rpw->SetResourceReference(&reference);
		rpw->Show();
	}
	ImGui::Separator();
}