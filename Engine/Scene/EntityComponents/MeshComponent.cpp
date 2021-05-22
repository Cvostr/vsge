#include "MeshComponent.hpp"
#include <Resources/ResourceCache.hpp>

using namespace VSGE;

void MeshComponent::SetMeshName(const std::string& mesh) {
	_meshResource.SetResource(mesh);
	//mMeshResource.mResourcePointer->Load();
}

void MeshComponent::Serialize(YAML::Emitter& e) {

}
void MeshComponent::Deserialize(YAML::Node& entity) {

}