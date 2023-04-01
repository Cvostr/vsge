#include "MeshComponent.hpp"
#include <Resources/ResourceCache.hpp>

using namespace VSGE;

void MeshComponent::SetMeshId(const Guid& id)
{
	_meshResource.SetResource(id);
}

void MeshComponent::SetMesh(const Guid& id, const std::string& childName)
{
	_meshResource.SetResource(id, childName);
}

void MeshComponent::SetMeshResource(MeshResource* resource) {
	_meshResource.SetResource(resource);
}

MeshResource* MeshComponent::GetMeshResource() {
	return (MeshResource*)_meshResource.GetResource(); 
}

Mesh* MeshComponent::GetMesh() {
	MeshResource* resource = GetMeshResource();
	if (resource) {
		return resource->GetMesh();
	}
	return nullptr;
}

ResourceReference& MeshComponent::GetResourceReference() {
	return _meshResource;
}

void MeshComponent::Serialize(ByteSerialize& serializer) 
{
	Resource* parentResource = _meshResource.GetParentResource();
	if (parentResource)
		serializer.Serialize(_meshResource.GetParentResource()->getId());
	else
		serializer.Serialize(Guid(0,0,0,0));

	serializer.Serialize(_meshResource.GetResource()->GetName());
}

void MeshComponent::Deserialize(ByteSolver& solver) 
{
	Guid parentId = solver.GetGuid();
	std::string childName = solver.ReadNextString();

	_meshResource.SetResource(parentId, childName);
}