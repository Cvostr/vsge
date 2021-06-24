#include "ResourcePickWindow.hpp"
#include <Resources/ResourceCache.hpp>
#include "BrowserWindow.hpp"
#include <ImageBtnText.h>
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

                if (reference->GetResourceType() == RESOURCE_TYPE_TEXTURE) {
                    //ImGui::SameLine();
                    FileBrowserWindow* fbw = ImGuiLayer::Get()->GetWindow<FileBrowserWindow>();
                    ImguiVulkanTexture* texture = fbw->GetCheckerboardTexture();

                    if (resource) {
                        if (resource->IsUnloaded())
                            resource->Load();
                        else
                            texture = fbw->GetTextureResource(resource->GetDataDescription().file_path);
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