#include "ResourceLayer.hpp"

using namespace VSGE;

void ResourceLayer::OnEvent(const VSGE::IEvent& event) {
	DispatchEvent<VSGE::ResourceLoadEvent>(event, EVENT_FUNC(ResourceLayer::OnResourceEvent));
}

void ResourceLayer::OnResourceEvent(const VSGE::ResourceLoadEvent& event) {
	if (event.GetResource()->GetResourceType() != RESOURCE_TYPE_MATERIAL
		&& event.GetResource()->GetResourceType() != RESOURCE_TYPE_ANIMATION
		&& event.GetResource()->GetResourceType() != RESOURCE_TYPE_AUDIOCLIP
		&& event.GetResource()->GetResourceType() != RESOURCE_TYPE_SCRIPT)
		return;

	event.GetResource()->PostLoad();
	event.GetResource()->SetState(RESOURCE_STATE_READY);
}

void ResourceLayer::OnAttach() {

}
void ResourceLayer::OnDetach() {

}
void ResourceLayer::OnUpdate() {

}