#include "MeshComponent.hpp"
#include <Resources/ResourceCache.hpp>

using namespace VSGE;
using namespace YAML;

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

void MeshComponent::Serialize(YAML::Emitter& e) {
	e << Key << "mesh_group" << Value << this->_meshResource.GetResourceParentName();
	e << Key << "mesh" << Value << this->_meshResource.GetResourceName();
}
void MeshComponent::Deserialize(YAML::Node& entity) {
	_meshResource.SetParentResource(entity["mesh_group"].as<std::string>());
	SetMeshName(entity["mesh"].as<std::string>());
}

void MeshComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_meshResource.GetResourceParentName());
	serializer.Serialize(_meshResource.GetResourceName());
}

void MeshComponent::Deserialize(ByteSolver& solver) {
	_meshResource.SetParentResource(solver.ReadNextString());
	SetMeshName(solver.ReadNextString());
}