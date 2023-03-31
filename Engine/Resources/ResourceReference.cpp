#include "ResourceReference.hpp"
#include "ResourceCache.hpp"

using namespace VSGE;

ResourceReference::ResourceReference() {
    _resourcePointer = nullptr;
    _resourceType = RESOURCE_TYPE_NONE;

	ResourceCache::Get()->AddResourceReference(this);
}

ResourceReference::~ResourceReference() {
	ResourceCache::Get()->RemoveResourceReference(this);
}

ResourceType ResourceReference::GetResourceType() {
	return _resourceType;
}

void ResourceReference::SetPointerToNull() {
	_resourcePointer = nullptr;
}

void ResourceReference::operator=(ResourceReference& ref) {
	SetResource(ref._resourceName);
    SetParentResource(ref._parentName);
}

void ResourceReference::SetResourceType(ResourceType type) {
	_resourceType = type;
}

Resource* ResourceReference::GetResource() {
	if (!_resourcePointer)
		_resourcePointer = ResourceCache::Get()->GetResource(_resourceName, _resourceType);
	return _resourcePointer;
}

Resource* ResourceReference::GetParentResource(ResourceType type) {
	if (_resourcePointer)
		if (_resourcePointer->GetParent())
			return _resourcePointer->GetParent();
	return ResourceCache::Get()->GetResource(_parentName, type);
}

const std::string& ResourceReference::GetResourceName() const {
	return _resourceName;
}

const Guid& ResourceReference::GetId() const
{
	return _resourceId;
}

bool ResourceReference::IsResourceSpecified() {
	return !_resourceName.empty();
}

void ResourceReference::SetResource(Resource* resource) {
	if (resource == nullptr)
		return;

	_resourceName = resource->GetName();
	_resourcePointer = ResourceCache::Get()->GetResource(_resourceName, _resourceType);
	_resourceId = resource->getId();
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
		SetResource(ResourceCache::Get()->GetResource(_resourceName, _resourceType));
}

void ResourceReference::SetResource(const Guid& id)
{
	_resourceId = id;
	SetResource(ResourceCache::Get()->GetResource(_resourceId));
}

const std::string& ResourceReference::GetResourceParentName() const{
    return _parentName;
}

void ResourceReference::SetParentResource(const std::string& parentName){
	_parentName = parentName;
}
