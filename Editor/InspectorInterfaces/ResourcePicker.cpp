#include "ResourcePicker.hpp"
#include "../Windows/BrowserWindow.hpp"
#include <Resources/ResourceCache.hpp>
#include "../Windows/ResourcePickWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGE;
using namespace VSGEditor;

void VSGEditor::DrawResourcePicker(std::string label, ResourceReference& reference, bool empty_res)
{
	ImGui::Text(label.c_str());

	ImGui::SameLine();

	if (reference.GetResourceType() == RESOURCE_TYPE_TEXTURE) {
		ImGui::SameLine();
		FileBrowserWindow* fbw = ImGuiLayer::Get()->GetWindow<FileBrowserWindow>();
		ImguiVulkanTexture* texture = fbw->GetCheckerboardTexture();

		Resource* resource = reference.GetResource();
		
		if (resource) {
			if (resource->IsUnloaded())
				resource->Load();
			else{
				texture = fbw->GetTextureResource(resource->GetDataDescription().file_path);
				resource->Use();
			}
		}

		if(texture)
			ImGui::Image((void*)texture->imtexture, ImVec2(64, 64));
	}else
		ImGui::Text(reference.GetResourceName().c_str());

	ImGui::SameLine();

	std::string btn_text = "Select " + label;

	if (ImGui::Button(btn_text.c_str())) {
		ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
		rpw->SetAllowEmptyResource(empty_res);
		rpw->SetResourceReference(&reference);
		rpw->Show();
	}

	/*if (ImGui::BeginPopup("resources"))
	{
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "--- Meshes ---");

		uint32 resources_size = ResourceCache::Get()->GetResourcesCount();

		for (uint32 resource_i = 0; resource_i < resources_size; resource_i++) {
			Resource* resource = ResourceCache::Get()->GetResources()[resource_i];
			if (resource->GetResourceType() == type) {
				if (ImGui::Button(resource->GetName().c_str())) {
					resource_name_out = resource->GetName();
				}
			}
		}

		ImGui::EndPopup();
	}*/

}