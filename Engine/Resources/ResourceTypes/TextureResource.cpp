#include "TextureResource.hpp"

using namespace VSGE;

TextureResource::TextureResource() {
	_texture = Texture::NewTexture();
}

TextureResource::~TextureResource() {
	delete _texture;
}

Texture* TextureResource::GetTexture() { return _texture; }

void TextureResource::OnRelease() {
    _texture->Destroy();
}

void TextureResource::PostLoad() {

}

void TextureResource::Prepare(){
	ProcessTexture(GetLoadedData(), GetDataDescription().size, _processed_texture);
	bool result = _texture->CreateFromProcessed(_processed_texture);
	if (!result) {
		FreeLoadedData();
		_description.size = 0;
		_processed_texture.Destroy();
		SetState(RESOURCE_STATE_UNLOADED);
		return;
	}
	_processed_texture.Destroy();
	SetState(RESOURCE_STATE_READY);
}