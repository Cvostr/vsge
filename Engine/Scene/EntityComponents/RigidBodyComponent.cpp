#include "RigidBodyComponent.hpp"

using namespace VSGE;

float RigidBodyComponent::GetMass() {
	return _mass;
}

void RigidBodyComponent::SetMass(float mass) {
	_mass = mass;
}