#include "Texture.hpp"
#include "Core/FileLoader.hpp"

using namespace VSGE;

Texture::Texture() :
    _maxWidth(0),
    _maxHeight(0),
    _mipLevels(0),
    _layers(1),
    _flags(false),
    _format(FORMAT_RGBA)
{}

Texture::~Texture() {
    Destroy();
}

void Texture::SetRenderTargetFlag(bool renderTarget) {
    _flags |= renderTarget * TEXTURE_FLAG_IS_RENDER_TARGET;
}
bool Texture::IsRenderTarget() {
    return _flags & TEXTURE_FLAG_IS_RENDER_TARGET;
}

void Texture::SetCubemap(bool cubemap) {
    _flags |= cubemap * TEXTURE_FLAG_IS_CUBEMAP;
}

bool Texture::IsCubemap() { 
    return _flags & TEXTURE_FLAG_IS_CUBEMAP;
}

void Texture::SetStorage(bool storage){
    _flags |= storage * TEXTURE_FLAG_IS_STORAGE;
}

bool Texture::IsStorage(){
    return _flags & TEXTURE_FLAG_IS_STORAGE;
}

void Texture::SetFlags(uint32 flags){
    _flags = flags;
}

bool Texture::CreateFromFile(std::string filePath) {
    byte* texture_data;
    uint32 size;
    if (!LoadFile(filePath, (char**)&texture_data, &size))
        return false;
    if (!CreateFromBuffer(texture_data, size))
        return false;
    delete[] texture_data;
    return true;
}

bool Texture::CreateFromBuffer(byte* data, uint32 size) {
    if (data == nullptr || size == 0)
        return false;

    ProcessedTexture processed_texture;
    ProcessTexture(data, size, processed_texture);
    CreateFromProcessed(processed_texture);

    return false;
}

bool Texture::CreateFromProcessed(const ProcessedTexture& texture) {
    Create(texture._width, texture._height, texture._format, 1, (uint32)texture._mip_levels.size());

    for (uint32 mip_i = 0; mip_i < texture._mip_levels.size(); mip_i++) {
        const TextureMipLevel* mip = &texture._mip_levels[mip_i];
        AddMipLevel(mip->_mip_data, mip->_mip_data_size, mip->_mip_width, mip->_mip_height, mip_i, 0);
    }

    SetReadyToUseInShaders();

    return true;
}