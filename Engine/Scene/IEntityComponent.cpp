#include "IEntityComponent.hpp"

#include "EntityComponents/MeshComponent.hpp"
#include "EntityComponents/AnimatorComponent.hpp"
#include "EntityComponents/MaterialComponent.hpp"
#include "EntityComponents/LightComponent.hpp"
#include "EntityComponents/ParticleEmitterComponent.hpp"
#include "EntityComponents/AudioSourceComponent.hpp"
#include "EntityComponents/RigidBodyComponent.hpp"
#include "EntityComponents/ColliderComponent.hpp"
#include "EntityComponents/LightProbeComponent.hpp"
#include "EntityComponents/TerrainComponent.hpp"

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

	switch (type) {
		case ENTITY_COMPONENT_MESH:
			component = new MeshComponent;
			break;
		case ENTITY_COMPONENT_MATERIAL:
			component = new MaterialComponent;
			break;
		case ENTITY_COMPONENT_ANIMATOR:
			component = new AnimatorComponent;
			break;
		case ENTITY_COMPONENT_LIGHTSOURCE:
			component = new LightsourceComponent;
			break;
		case ENTITY_COMPONENT_PARTICLE_EMITTER:
			component = new ParticleEmitterComponent;
			break;
		case ENTITY_COMPONENT_AUDIO_SOURCE:
			component = new AudioSourceComponent;
			break;
		case ENTITY_COMPONENT_RIGIDBODY:
			component = new RigidBodyComponent;
			break;
		case ENTITY_COMPONENT_COLLIDER:
			component = new ColliderComponent;
			break;
		case ENTITY_COMPONENT_CAMERA:
			component = new Camera;
			break;
		case ENTITY_COMPONENT_ENV_PROBE:
			component = new LightProbeComponent;
			break;
		case ENTITY_COMPONENT_TERRAIN:
			component = new TerrainComponent;
			break;
	}

	return component;
}