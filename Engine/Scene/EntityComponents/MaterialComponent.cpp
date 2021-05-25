#include "MaterialComponent.hpp"

using namespace YAML;
using namespace VSGE;

void MaterialComponent::Serialize(YAML::Emitter& e) {
	e << Key << "material" << Value << this->_materialResource.GetResourceName();
}
void MaterialComponent::Deserialize(YAML::Node& entity) {
	SetMaterialName(entity["material"].as<std::string>());
}