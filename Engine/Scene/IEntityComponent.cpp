#include "IEntityComponent.hpp"

#include "EntityComponents/MeshComponent.hpp"
#include "EntityComponents/AnimatorComponent.hpp"
#include "EntityComponents/MaterialComponent.hpp"
#include "EntityComponents/LightComponent.hpp"
#include "EntityComponents/ParticleEmitterComponent.hpp"
#include "EntityComponents/AudioSourceComponent.hpp"
#include "EntityComponents/RigidBodyComponent.hpp"
#include "EntityComponents/ColliderComponent.hpp"

using namespace VSGE;

IEntityComponent::IEntityComponent() :
	_active(true), 
	_entity(nullptr)
{}

void IEntityComponent::SetActive(bool active) {
	if (active && !_active) {
		OnActivate();
	}
	if (!active && _active) {
		OnDeactivate();
	}

	_active = active;
}

bool IEntityComponent::IsActive() const { 
	return _active; 
}

void IEntityComponent::SetEntity(Entity* entity) { 
	_entity = entity; 
}

Entity* IEntityComponent::GetEntity() const {
	return _entity;
}

IEntityComponent* VSGE::CreateEntityComponent(EntityComponentType type) {
	IEntityComponent* component = nullptr;
	if (type == ENTITY_COMPONENT_MESH) {
		component = new MeshComponent;
	}
	if (type == ENTITY_COMPONENT_MATERIAL) {
		component = new MaterialComponent;
	}
	if (type == ENTITY_COMPONENT_ANIMATOR) {
		component = new AnimatorComponent;
	}
	if (type == ENTITY_COMPONENT_LIGHTSOURCE) {
		component = new LightsourceComponent;
	}
	if (type == ENTITY_COMPONENT_PARTICLE_EMITTER) {
		component = new ParticleEmitterComponent;
	}
	if (type == ENTITY_COMPONENT_AUDIO_SOURCE) {
		component = new AudioSourceComponent;
	}
	if (type == ENTITY_COMPONENT_RIGIDBODY) {
		component = new RigidBodyComponent;
	}
	if (type == ENTITY_COMPONENT_COLLIDER) {
		component = new ColliderComponent;
	}
	if (type == ENTITY_COMPONENT_CAMERA) {
		component = new Camera;
	}

	return component;
}