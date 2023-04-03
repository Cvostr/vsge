#include "ResourceReference.hpp"
#include "ResourceCache.hpp"

using namespace VSGE;

ResourceReference::ResourceReference() {
    _resourcePointer = nullptr;
	_resourceId = Guid(0, 0, 0, 0);

	ResourceCache::Get()->AddResourceReference(this);
}

ResourceReference::~ResourceReference() 
{
	ResourceCache::Get()->RemoveResourceReference(this);
}

void ResourceReference::SetPointerToNull() {
	_resourcePointer = nullptr;
}

Resource* ResourceReference::ResolvePointer() const
{
	if (!_resourceId.isValid()) {
		return ResourceCache::Get()->GetResource(_childName);
	}

	Resource* resource = ResourceCache::Get()->GetResource(_resourceId);
	if (!_childName.empty())
	{
		//Указано имя дочернего ресурса
		resource = resource->GetSubresourceWithName(_childName);
	}

	return resource;
}

void ResourceReference::UpdatePointer()
{
	if (!_resourcePointer)
		_resourcePointer = ResolvePointer();
}

Resource* ResourceReference::GetResource() 
{
	UpdatePointer();
	return _resourcePointer;
}

Resource* ResourceReference::GetParentResource()
{
	return GetResource()->GetParent();
}

const std::string ResourceReference::GetResourceName() const 
{
	Resource* ptr = _resourcePointer != nullptr ? _resourcePointer : ResolvePointer();
	std::string resourceName = ptr != nullptr ? ptr->GetName() : "";

	return resourceName;
}

const Guid& ResourceReference::GetId() const
{
	return _resourceId;
}

bool ResourceReference::IsResourceSpecified() 
{
	return _resourceId.isValid();
}

void ResourceReference::SetResource(Resource* resource)
{
	if (resource == nullptr)
		return;

	if (resource->GetParent())
	{
		//Это дочерний ресурс, запоминаем имя
		_childName = resource->GetName();
		_resourceId = resource->GetParent()->getId();
	}
	else 
	{
		_childName = "";
		_resourceId = resource->getId();
	}

	_resourcePointer = nullptr;
	UpdatePointer();
}

void ResourceReference::SetResource(const Guid& parentId, const std::string& childName)
{
	_resourcePointer = nullptr;
	_childName = childName;
	_resourceId = parentId;
	UpdatePointer();
}

void ResourceReference::SetResource(const Guid& id)
{
	SetResource(id, "");
}