#include "MeshComponent.hpp"
#include <Resources/ResourceCache.hpp>

using namespace VSGE;
using namespace YAML;

void MeshComponent::SetMeshName(const std::string& mesh) {
	_meshResource.SetResource(mesh);
}

MeshResource* MeshComponent::GetMeshResource() {
	return _meshResource.GetResource<MeshResource>(); 
}

ResourceReference& MeshComponent::GetResourceReference() {
	return _meshResource;
}

void MeshComponent::Serialize(YAML::Emitter& e) {
	e << Key << "mesh" << Value << this->_meshResource.GetResourceName();
}
void MeshComponent::Deserialize(YAML::Node& entity) {
	SetMeshName(entity["mesh"].as<std::string>());
}

void MeshComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(this->_meshResource.GetResourceName());
}

void MeshComponent::Deserialize(ByteSolver& solver) {
	SetMeshName(solver.ReadNextString());
}