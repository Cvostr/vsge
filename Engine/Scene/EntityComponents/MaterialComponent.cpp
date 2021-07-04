#include "MaterialComponent.hpp"

using namespace YAML;
using namespace VSGE;

MaterialComponent::MaterialComponent() {
	_materialResource.SetResourceType(RESOURCE_TYPE_MATERIAL);
}

MaterialResource* MaterialComponent::GetMaterialResource() {
	return _materialResource.GetResource<MaterialResource>();
}

ResourceReference& MaterialComponent::GetResourceReference() {
	return _materialResource;
}

void MaterialComponent::SetMaterialName(const std::string& mesh) {
	_materialResource.SetResource(mesh);
}

void MaterialComponent::Serialize(YAML::Emitter& e) {
	e << Key << "material" << Value << this->_materialResource.GetResourceName();
}
void MaterialComponent::Deserialize(YAML::Node& entity) {
	SetMaterialName(entity["material"].as<std::string>());
}

void MaterialComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(this->_materialResource.GetResourceName());
}

void MaterialComponent::Deserialize(ByteSolver& solver) {
	SetMaterialName(solver.ReadNextString());
}