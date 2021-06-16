#include "ResourceLayer.hpp"

using namespace VSGE;

void ResourceLayer::OnEvent(const VSGE::IEvent& event) {
	DispatchEvent<VSGE::ResourceLoadEvent>(event, EVENT_FUNC(ResourceLayer::OnResourceEvent));
}

void ResourceLayer::OnResourceEvent(const VSGE::ResourceLoadEvent& event) {
	if (event.GetResource()->GetResourceType() != RESOURCE_TYPE_TEXTURE
		&& event.GetResource()->GetResourceType() != RESOURCE_TYPE_MATERIAL
		&& event.GetResource()->GetResourceType() != RESOURCE_TYPE_ANIMATION)
		return;

	event.GetResource()->PostLoad();
	event.GetResource()->FreeLoadedData();
	event.GetResource()->SetState(RESOURCE_STATE_READY);
}

void ResourceLayer::OnAttach() {

}
void ResourceLayer::OnDetach() {

}
void ResourceLayer::OnUpdate() {

}