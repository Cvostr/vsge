#include "TextureResource.hpp"

using namespace VSGE;

TextureResource::TextureResource() {
	_texture = CreateTexture();
	_isRenderTarget = false;
}

TextureResource::~TextureResource() {
	delete _texture;
}

Texture* TextureResource::GetTexture() { return _texture; }

void TextureResource::OnRelease() {
    _texture->Destroy();
}

void TextureResource::PostLoad() {
	_texture->CreateFromProcessed(_processed_texture);
}

void TextureResource::Prepare(){
	_processed_texture.Destroy();
	ProcessTexture(GetLoadedData(), GetDataDescription().size, _processed_texture);
}