#include "MaterialComponent.hpp"

using namespace VSGE;

MaterialComponent::MaterialComponent() {
	_materialResource.SetResourceType(RESOURCE_TYPE_MATERIAL);
	_cast_shadows = true;
}

MaterialResource* MaterialComponent::GetMaterialResource() {
	return _materialResource.GetResource<MaterialResource>();
}

Material* MaterialComponent::GetMaterial() {
	if (GetMaterialResource())
		return GetMaterialResource()->GetMaterial();
	return nullptr;
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

void MaterialComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_materialResource.GetResourceName());
	serializer.Serialize(_cast_shadows);
}

void MaterialComponent::Deserialize(ByteSolver& solver) {
	SetMaterialName(solver.ReadNextString());
	_cast_shadows = solver.GetValue<bool>();
}