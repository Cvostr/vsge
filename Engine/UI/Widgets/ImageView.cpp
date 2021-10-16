#include "ImageView.hpp"
#include "../UiLayer.hpp"
#include <Resources/ResourceTypes/TextureResource.hpp>

using namespace VSGE;

SpriteView::SpriteView() {
	_texture_reference.SetResourceType(RESOURCE_TYPE_TEXTURE);
}
SpriteView::~SpriteView() {

}
void SpriteView::Draw() {
	UiLayer::Get()->GetRenderList()->DrawSprite(_transform, _bounds, _texture_reference.GetResource<TextureResource>());
}

void SpriteView::SetSprite(const std::string& sprite) {
	_texture_reference.SetResource(sprite);
}