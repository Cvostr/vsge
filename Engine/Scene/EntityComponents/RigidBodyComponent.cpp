#include "RigidBodyComponent.hpp"
#include <yaml-cpp/yaml.h>

using namespace YAML;
using namespace VSGE;

RigidBodyComponent::RigidBodyComponent() {
	_mass = 1.f;
	_rigidBody = nullptr;
}

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

void RigidBodyComponent::ClearForces() {
	if (!_rigidBody)
		return;

	_rigidBody->clearForces();
}

void RigidBodyComponent::ApplyCentralImpulse(const Vec3& impulse) {
	if (!_rigidBody) 
		return;

	_rigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
}
void RigidBodyComponent::ApplyCentralForce(const Vec3& force) {
	if (!_rigidBody) 
		return;

	_rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
}