#include "ColliderComponent.hpp"
#include <yaml-cpp/yaml.h>

using namespace YAML;
using namespace VSGE;

ColliderShape ColliderComponent::GetShape() {
	return _shape;
}

void ColliderComponent::SetColliderShape(ColliderShape shape) {
	_shape = shape;
}

void ColliderComponent::Serialize(YAML::Emitter& e) {
	e << Key << "shape" << Value << _shape;
}

void ColliderComponent::Deserialize(YAML::Node& entity) {
	_shape = (ColliderShape)entity["shape"].as<int>();
}