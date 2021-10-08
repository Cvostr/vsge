#include "MaterialComponent.hpp"

using namespace YAML;
using namespace VSGE;

MaterialComponent::MaterialComponent() {
	_materialResource.SetResourceType(RESOURCE_TYPE_MATERIAL);
	_cast_shadows = true;
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

bool MaterialComponent::IsCastShadows() {
	return _cast_shadows;
}
void MaterialComponent::SetCastShadows(bool cast_shadows) {
	_cast_shadows = cast_shadows;
}

void MaterialComponent::Serialize(YAML::Emitter& e) {
	e << Key << "material" << Value << _materialResource.GetResourceName();
	e << Key << "cast_shadows" << Value << _cast_shadows;
}
void MaterialComponent::Deserialize(YAML::Node& entity) {
	SetMaterialName(entity["material"].as<std::string>());
	_cast_shadows = entity["cast_shadows"].as<bool>();
}

void MaterialComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_materialResource.GetResourceName());
	serializer.Serialize(_cast_shadows);
}

void MaterialComponent::Deserialize(ByteSolver& solver) {
	SetMaterialName(solver.ReadNextString());
	_cast_shadows = solver.GetValue<bool>();
}