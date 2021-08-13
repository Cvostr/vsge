#include "TextureResource.hpp"

using namespace VSGE;

TextureResource::TextureResource() {
	_texture = CreateTexture();
}

TextureResource::~TextureResource() {
	delete _texture;
}

Texture* TextureResource::GetTexture() { return _texture; }

void TextureResource::Release() {
    _texture->Destroy();
}

void TextureResource::PostLoad() {
	_texture->CreateFromBuffer(_loadedData, _description.size);
}