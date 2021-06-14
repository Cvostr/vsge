#include "RigidBodyComponent.hpp"
#include <yaml-cpp/yaml.h>

using namespace YAML;
using namespace VSGE;

RigidBodyComponent::RigidBodyComponent() {
	_mass = 1.f;
	_friction = 0.5f;
	_restitution = 0.0f;

	_rigidBody = nullptr;
	_collision_shape = nullptr;
}

float RigidBodyComponent::GetMass() {
	return _mass;
}

void RigidBodyComponent::SetMass(float mass) {
	if (mass >= 0 && _mass != mass) {
		_mass = mass;

		if (_rigidBody) {
			_rigidBody->setMassProps(mass, _rigidBody->getLocalInertia());
		}
	}
}

void RigidBodyComponent::Serialize(YAML::Emitter& e) {
	e << Key << "mass" << Value << _mass;
}

void RigidBodyComponent::Deserialize(YAML::Node& entity) {
	_mass = entity["mass"].as<float>();
}

void RigidBodyComponent::Activate() {
	if (!_rigidBody)
		return;

	_rigidBody->activate(true);
}

void RigidBodyComponent::Deactivate() {
	if (!_rigidBody)
		return;

	_rigidBody->setActivationState(WANTS_DEACTIVATION);
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

void RigidBodyComponent::AddToWorld() {

}