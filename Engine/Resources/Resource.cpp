#include "Resource.hpp"
#include "ResourceCache.hpp"
#include <Core/Time.hpp>

using namespace VSGE;

Resource* Resource::GetParent() {
	return _parent;
}

void Resource::SetParent(Resource* resource) {
	_parent = resource;
}

void Resource::SetDataDescription(const DataDescription& desc) {
	_description = desc;
}

const DataDescription& Resource::GetDataDescription() {
	return _description;
}

byte* Resource::GetLoadedData() {
	return _loadedData;
}

void Resource::SetLoadedData(byte* data) {
	_loadedData = data;
}

void Resource::Use() {
	_lastUseFrame = TimePerf::Get()->GetCurrentTime();
}

void Resource::Load(bool just_read) {
	if (_resourceState == RESOURCE_STATE_UNLOADED) {
		ResourceCache::Get()->GetAsyncLoader()->AddToQueue(this);
	}
}

void Resource::FreeLoadedData() {
	SAFE_RELEASE_ARR(_loadedData)
}

Resource* ResourceReference::GetResource() {
	return _resourcePointer;
}

const std::string& ResourceReference::GetResourceName() const {
	return _resourceName;
}

bool ResourceReference::IsResourceSpecified() {
	return !_resourceName.empty();
}

void ResourceReference::SetResource(Resource* resource) {
	if (resource == nullptr)
		return;

	_resourceName = resource->GetName();
	_resourcePointer = ResourceCache::Get()->GetResource(_resourceName);
	SetResourceType(resource->GetResourceType());

	//if resource created from other resource
	if (resource->GetParent()) {
		_parentName = resource->GetParent()->GetName();
	}
}

void ResourceReference::SetResource(const std::string& resourceName) {
	_resourceName = resourceName;
	if (resourceName.empty())
		_resourcePointer = nullptr;
	else
		SetResource(ResourceCache::Get()->GetResource(_resourceName));
}
