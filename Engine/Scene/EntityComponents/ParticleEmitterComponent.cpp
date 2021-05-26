#include "ParticleEmitterComponent.hpp"

using namespace VSGE;

ParticleEmitterComponent::ParticleEmitterComponent() :
	_simulating(false),
	_shape(PE_SHAPE_SPHERE),
	_duration(5),
	_looping(true),
	_prewarm(true),
	_lifetime(2),
	_maxParticles(100),
	_direction(Vec3(0.f, 1.f, 0.f)),
	_velocity(8.f, 10.f),
	_constantForce(0, -9.8f, 0),
	_dampingForce(0),
	_rotation(0, 0),
	_rotationSpeed(0, 0),
	_emissionRate(200, 200)
{
	_meshResource.SetResourceType(RESOURCE_TYPE_MESH);
	_materialResource.SetResourceType(RESOURCE_TYPE_MATERIAL);
}

void ParticleEmitterComponent::Serialize(YAML::Emitter& e) {

}
void ParticleEmitterComponent::Deserialize(YAML::Node& entity) {

}