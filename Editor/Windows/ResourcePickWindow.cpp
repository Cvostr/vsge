#include "ResourcePickWindow.hpp"
#include <Resources/ResourceCache.hpp>
#include <ImageBtnText.h>
#include "../EditorLayers/ImGuiLayer.hpp"
#include "../Misc/Thumbnails.hpp"
#include "../Misc/VkMaterialsThumbnails.hpp"
#include "MonoScripting/MonoScriptStorage.hpp"

using namespace VSGEditor;
using namespace VSGE;

ResourcePickerWindow::ResourcePickerWindow() {
    SetSize(600, 600);
    _showCloseCross = true;
    _allow_empty_resource = false;
    reference = nullptr;
    script_reference = nullptr;
}

void ResourcePickerWindow::OnDrawWindow() {
    if (Draw("Select")) {
        
        if (reference)
            DrawResourcePicker();
        if (script_reference)
            DrawScriptPicker();
        
        ImGui::End();
    }
}

void ResourcePickerWindow::DrawResourcePicker() {
    uint32 resources_size = ResourceCache::Get()->GetResourcesCount();
    float width = ImGui::GetCurrentWindow()->Size.x;
    uint32 drawn_pix = 0;

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

                ImguiVulkanTexture* tex = TextureThumbnails::Get()->GetTextureResource(resource->GetDataDescription().file_path);
                if (tex)
                    texture = tex;

                if (texture) {
                    unsigned int pix = 0;
                    bool hovered = false;
                    bool clicked = ImageButtonWithText((void*)texture->imtexture, resource->GetName().c_str(), &pix, &hovered, ImVec2(64, 64));
                    if (clicked) {
                        SetResourceToReference(resource);
                    }

                    //Summarize pixels
                    drawn_pix += pix;
                    //If we have enough space
                    if ((width - drawn_pix) > 256)
                        //Then draw next in same line
                        ImGui::SameLine();
                    else
                        drawn_pix = 0;
                }
            }
            else if (reference->GetResourceType() == RESOURCE_TYPE_MATERIAL) {
                ImTextureID texture = TextureThumbnails::Get()->GetCheckerboardTexture()->imtexture;

                if (resource->GetState() != RESOURCE_STATE_READY) {
                    resource->Load();
                }

                ImTextureID thumb = VkMaterialsThumbnails::Get()->GetMaterialThumbnailTexture(resource->GetName());
                if (resource->IsReady())
                    VkMaterialsThumbnails::Get()->CreateThumbnail(resource->GetName());
                if (thumb)
                    texture = thumb;


                if (texture) {
                    unsigned int pix = 0;
                    bool hovered = false;
                    bool clicked = ImageButtonWithText((void*)texture, resource->GetName().c_str(), &pix, &hovered, ImVec2(64, 64));
                    if (clicked) {
                        SetResourceToReference(resource);
                    }

                    //Summarize pixels
                    drawn_pix += pix;
                    //If we have enough space
                    if ((width - drawn_pix) > 256)
                        //Then draw next in same line
                        ImGui::SameLine();
                    else
                        drawn_pix = 0;
                }
            }
            else if (ImGui::Button(resource->GetName().c_str())) {
                SetResourceToReference(resource);
            }
        }
    }
}
void ResourcePickerWindow::DrawScriptPicker() {
    MonoScriptStorage* storage = MonoScriptStorage::Get();

    for (auto& script : storage->GetScripts()) {
        if (ImGui::Button(script->GetClassName().c_str())) {
            script_reference->SetClassName(script->GetClassName());
            Hide();
        }
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
    script_reference = nullptr;
}

void ResourcePickerWindow::SetScriptReference(VSGE::EntityScriptComponent* script) {
    script_reference = script;
    reference = nullptr;
}