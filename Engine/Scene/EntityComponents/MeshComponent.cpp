#include "MeshComponent.hpp"
#include <Resources/ResourceCache.hpp>

using namespace VSGE;

void MeshComponent::SetMeshName(const std::string& mesh) {
	_meshResource.SetResource(mesh);
}

void MeshComponent::SetMeshResource(MeshResource* resource) {
	_meshResource.SetResource(resource);
}

MeshResource* MeshComponent::GetMeshResource() {
	return _meshResource.GetResource<MeshResource>(); 
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

void MeshComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_meshResource.GetResourceParentName());
	serializer.Serialize(_meshResource.GetResourceName());
}

void MeshComponent::Deserialize(ByteSolver& solver) {
	_meshResource.SetParentResource(solver.ReadNextString());
	SetMeshName(solver.ReadNextString());
}