#include "MaterialResource.hpp"

using namespace VSGE;

MaterialResource::MaterialResource() {
	_material = new Material;
	_material->SetTemplate("default_pbr");
}

MaterialResource::~MaterialResource() {
	delete _material;
}

Material* MaterialResource::GetMaterial() { 
	return _material; 
}

void MaterialResource::OnRelease() {
	_material->Destroy();
}

void MaterialResource::PostLoad() {
	_material->Deserialize(_loadedData, _description.size);
}