#include "ResourcePicker.hpp"
#include <Resources/ResourceCache.hpp>
#include "../Windows/ResourcePickWindow.hpp"
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGE;
using namespace VSGEditor;

void VSGEditor::DrawResourcePicker(std::string label, ResourceReference& reference)
{
	ImGui::Text(label.c_str());

	ImGui::SameLine();

	ImGui::Text(reference.GetResourceName().c_str());

	ImGui::SameLine();

	if (ImGui::Button("Select")) {
		ResourcePickerWindow* rpw = ImGuiLayer::Get()->GetWindow<ResourcePickerWindow>();
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