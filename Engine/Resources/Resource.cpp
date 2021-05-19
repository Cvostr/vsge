#include "Resource.hpp"
#include "ResourceCache.hpp"

using namespace VSGE;

void Resource::Load() {
	if (mResourceState == RESOURCE_STATE_UNLOADED) {
		ResourceCache::Get()->GetAsyncLoader()->AddToQueue(this);
	}
	else if (mResourceState == RESOURCE_STATE_LOADED) {
		PostLoad();
		SetState(RESOURCE_STATE_READY);
		FreeLoadedData();
	}
}

void Resource::FreeLoadedData() {
	if (mLoadedData) {
		delete[] mLoadedData;
		mLoadedData = nullptr;
	}
}

void ResourceReference::SetResource(const std::string& resourceName) {
	_resourceName = resourceName;
	_resourcePointer = ResourceCache::Get()->GetResource(_resourceName);
}