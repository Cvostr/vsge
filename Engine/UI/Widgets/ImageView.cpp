#include "ImageView.hpp"
#include "../UiLayer.hpp"
#include <Resources/ResourceTypes/TextureResource.hpp>

using namespace VSGE;

SpriteView::SpriteView() {

}

SpriteView::~SpriteView() {

}
void SpriteView::Draw() {
	UiLayer::Get()->GetRenderList()->DrawSprite(_global_bounds, _rotation, _texture_reference.GetResource<TextureResource>());
}

void SpriteView::SetSprite(const std::string& sprite) {
	_texture_reference.SetResource(ResourceCache::Get()->GetResource(sprite));
}