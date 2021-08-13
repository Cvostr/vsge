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
	_texture->CreateFromBuffer(_loadedData, _description.size);
}

void TextureResource::Prepare(){
	//PostLoad();
	//SetState(RESOURCE_STATE_READY);
}