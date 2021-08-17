#include "ResourcePickWindow.hpp"
#include <Resources/ResourceCache.hpp>
#include <ImageBtnText.h>
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../Misc/Thumbnails.hpp"

using namespace VSGEditor;
using namespace VSGE;

ResourcePickerWindow::ResourcePickerWindow() {
    SetSize(600, 600);
    _showCloseCross = true;
}

void ResourcePickerWindow::OnDrawWindow() {
    if (Draw("Select")) {
        uint32 resources_size = ResourceCache::Get()->GetResourcesCount();

        if (reference == nullptr)
            resources_size = 0;

        if (_allow_empty_resource) {
            if (ImGui::Button("...", ImVec2(32, 32))) {
                SetResourceToReference("");
            }
        }

        for (uint32 resource_i = 0; resource_i < resources_size; resource_i++) {
            Resource* resource = ResourceCache::Get()->GetResources()[resource_i];
            if (resource->GetResourceType() == reference->GetResourceType()) {

                if (reference->GetResourceType() == RESOURCE_TYPE_TEXTURE) {
                    ImguiVulkanTexture* texture = TextureThumbnails::Get()->GetCheckerboardTexture();

                    if (resource) {
                        ImguiVulkanTexture* tex = TextureThumbnails::Get()->GetTextureResource(resource->GetDataDescription().file_path);
                        if (tex)
                            texture = tex;
                    }

                    if (texture) {
                        unsigned int pix = 0;
                        bool hovered = false;
                        bool clicked = ImageButtonWithText((void*)texture->imtexture, resource->GetName().c_str(), &pix, &hovered, ImVec2(64, 64));
                        if (clicked) {
                            SetResourceToReference(resource);
                        }
                    }
                }else if (ImGui::Button(resource->GetName().c_str())) {
                    SetResourceToReference(resource);
                }
            }
        }
        
        ImGui::End();
    }
}

void ResourcePickerWindow::SetResourceToReference(Resource* resource) {
    reference->SetResource(resource->GetName());
    Hide();
}

void ResourcePickerWindow::SetResourceToReference(const std::string& resource_name) {
    reference->SetResource(resource_name);
    Hide();
}

void ResourcePickerWindow::SetAllowEmptyResource(bool empty_resource) {
    _allow_empty_resource = empty_resource;
}

void ResourcePickerWindow::SetResourceReference(VSGE::ResourceReference* reference) {
    this->reference = reference;
    script = nullptr;
}

void ResourcePickerWindow::SetScriptReference(VSGE::EntityScriptComponent* script) {
    this->reference = nullptr;
    this->script = script;
}