#include "RigidBodyComponent.hpp"
#include <yaml-cpp/yaml.h>

using namespace YAML;
using namespace VSGE;

float RigidBodyComponent::GetMass() {
	return _mass;
}

void RigidBodyComponent::SetMass(float mass) {
	_mass = mass;
}

void RigidBodyComponent::Serialize(YAML::Emitter& e) {
	e << Key << "mass" << Value << _mass;
}

void RigidBodyComponent::Deserialize(YAML::Node& entity) {
	_mass = entity["mass"].as<float>();
}