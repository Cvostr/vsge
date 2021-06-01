#include "ResourcePickWindow.hpp"
#include <Resources/ResourceCache.hpp>
#include "../EditorLayers/ImGuiLayer.hpp"

using namespace VSGEditor;
using namespace VSGE;

ResourcePickerWindow::ResourcePickerWindow() {
    SetSize(600, 600);
    _showCloseCross = true;
}

void ResourcePickerWindow::OnDrawWindow() {
    if (Draw("Select")) {
        uint32 resources_size = ResourceCache::Get()->GetResourcesCount();

        for (uint32 resource_i = 0; resource_i < resources_size; resource_i++) {
            Resource* resource = ResourceCache::Get()->GetResources()[resource_i];
            if (resource->GetResourceType() == reference->GetResourceType()) {
                if (ImGui::Button(resource->GetName().c_str())) {
                    reference->SetResource(resource->GetName());
                    Hide();
                }
            }
        }

        ImGui::End();
    }
}