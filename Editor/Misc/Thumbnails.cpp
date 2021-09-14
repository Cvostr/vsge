#include "Thumbnails.hpp"
#include <Resources/ResourceCache.hpp>
#include <Resources/ResourceTypes/TextureResource.hpp>

using namespace VSGE;

TextureThumbnails* TextureThumbnails::_this = new TextureThumbnails;

TextureThumbnails::TextureThumbnails() {
    _this = this;
    sampler = nullptr;
    _emptyTextureIcon = nullptr;
}

ImguiVulkanTexture* TextureThumbnails::GetTextureResource(const std::string& fname) {
    if (!sampler) {
        sampler = new VulkanSampler;
        sampler->Create();
    }

    if (!_emptyTextureIcon) {
        _emptyTextureIcon = new ImguiVulkanTexture;
        _emptyTextureIcon->CreateFromFile("res/icons/Checkerboard.png", *sampler);
    }

    STRIMGVKT* ptr = nullptr;
    for (auto& texture_res : _textureResources) {
        if (texture_res->first == fname) {
            ptr = texture_res;
        }
    }

    if (ptr == nullptr) {
        ptr = new STRIMGVKT;
        ptr->first = fname;
        ptr->second = nullptr;
        _textureResources.push_back(ptr);
    }

    if (ptr != nullptr) {
        if (ptr->second != nullptr) {
            TextureResource* texture = (TextureResource*)ResourceCache::Get()->GetResourceWithFilePath(fname);
            texture->Use();
            if (texture->IsUnloaded()) {
                ptr->second = nullptr;
            }
        }

        if (ptr->second == nullptr) {
            TextureResource* texture = (TextureResource*)ResourceCache::Get()->GetResourceWithFilePath(fname);
            if (texture) {
                if (texture->IsUnloaded()) {
                    texture->Load();
                }
                if (texture->GetState() == RESOURCE_STATE_READY) {
                    ptr->second = new ImguiVulkanTexture;
                    ptr->second->texture = (VulkanTexture*)texture->GetTexture();
                    ptr->second->CreateImgui(*sampler);
                }
            }
        }
    }

    return ptr->second;
}