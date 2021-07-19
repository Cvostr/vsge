#include "ResourcePickWindow.hpp"
#include <Resources/ResourceCache.hpp>
#include <Scripting/Angel/AngelScriptLayer.hpp>
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
        uint32 scripts_size = AngelScriptLayer::Get()->GetModule()->GetMainClassDescs().size();

        if (reference == nullptr)
            resources_size = 0;

        if (script == nullptr)
            scripts_size = 0;

        for (uint32 resource_i = 0; resource_i < resources_size; resource_i++) {
            Resource* resource = ResourceCache::Get()->GetResources()[resource_i];
            if (resource->GetResourceType() == reference->GetResourceType()) {

                if (reference->GetResourceType() == RESOURCE_TYPE_TEXTURE) {
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

        for (uint32 script_i = 0; script_i < scripts_size; script_i++) {
            MainClassDesc* class_desc = AngelScriptLayer::Get()->GetModule()->GetMainClassDescs()[script_i];

            if (ImGui::Button(class_desc->_name.c_str())) {
                script->SetScriptName(class_desc->_name);
                Hide();
            }
        }

        ImGui::End();
    }
}

void ResourcePickerWindow::SetResourceToReference(Resource* resource) {
    reference->SetResource(resource->GetName());
    Hide();
}

void ResourcePickerWindow::SetResourceReference(VSGE::ResourceReference* reference) {
    this->reference = reference;
    script = nullptr;
}

void ResourcePickerWindow::SetScriptReference(VSGE::EntityScriptComponent* script) {
    this->reference = nullptr;
    this->script = script;
}