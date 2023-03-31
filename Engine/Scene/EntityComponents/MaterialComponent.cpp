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

void MaterialComponent::SetMaterialResource(MaterialResource* resource)
{
	_materialResource.SetResource(resource);
}

void MaterialComponent::SetMaterialId(const Guid& material)
{
	_materialResource.SetResource(material);
}

bool MaterialComponent::IsCastShadows() 
{
	return _cast_shadows;
}

void MaterialComponent::SetCastShadows(bool cast_shadows) 
{
	_cast_shadows = cast_shadows;
}

void MaterialComponent::Serialize(ByteSerialize& serializer) 
{
	serializer.Serialize(_materialResource.GetId());
	serializer.Serialize(_cast_shadows);
}

void MaterialComponent::Deserialize(ByteSolver& solver) 
{
	Guid id = solver.GetGuid();
	_materialResource.SetResource(id);
	_cast_shadows = solver.GetValue<bool>();
}