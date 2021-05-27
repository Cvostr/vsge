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

void ParticleEmitterComponent::OnPreRender() {

}

void ParticleEmitterComponent::StartSimulation() {
	_particles.resize(_maxParticles);

	for (uint32 particle_i = 0; particle_i < _maxParticles; particle_i++) {
		_particles[particle_i] = new Particle;
	}

	_simulating = true;
	_simulationTime = 0;
	_emitterTime = 0;
}

void ParticleEmitterComponent::StopSimulation() {
	//Free all particles
	for (auto particle : _particles) {
		delete particle;
	}
	//Clear particles vector
	_particles.clear();

	_simulating = false;
}

void ParticleEmitterComponent::RestartSimulation() {
	StopSimulation();
	StartSimulation();
}

void ParticleEmitterComponent::DestroyParticle(Particle* particle) {
	particle->_alive = false;
}

uint32 ParticleEmitterComponent::GetAliveParticlesCount() {
	uint32_t Result = 0;
	for (auto particle : _particles) {
		if (particle->_alive)
			Result += 1;
	}
	return Result;
}

uint32 ParticleEmitterComponent::GetFreeParticleIndex() {
	for (uint32_t i = 0; i < static_cast<uint32_t>(_particles.size()); i++) {
		if (!_particles[i]->_alive)
			return i;
	}
	return UINT64_MAX;
}