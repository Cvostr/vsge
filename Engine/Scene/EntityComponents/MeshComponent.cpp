#include "MeshComponent.hpp"
#include <Resources/ResourceCache.hpp>

using namespace VSGE;
using namespace YAML;

void MeshComponent::SetMeshName(const std::string& mesh) {
	_meshResource.SetResource(mesh);
}

void MeshComponent::Serialize(YAML::Emitter& e) {
	e << Key << "mesh" << Value << this->_meshResource.GetResourceName();
}
void MeshComponent::Deserialize(YAML::Node& entity) {
	SetMeshName(entity["mesh"].as<std::string>());
}