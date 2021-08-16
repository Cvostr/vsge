#include "ResourceReference.hpp"
#include "ResourceCache.hpp"

using namespace VSGE;

ResourceReference::ResourceReference() {
    _resourcePointer = nullptr;
    _resourceType = RESOURCE_TYPE_NONE;
}

ResourceType ResourceReference::GetResourceType() {
	return _resourceType;
}

void ResourceReference::operator=(ResourceReference& ref) {
	SetResource(ref._resourceName);
    SetParentResource(ref._parentName);
}

void ResourceReference::SetResourceType(ResourceType type) {
	_resourceType = type;
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

const std::string& ResourceReference::GetResourceParentName() const{
    return _parentName;
}

void ResourceReference::SetParentResource(const std::string& parentName){
	_parentName = parentName;
}
