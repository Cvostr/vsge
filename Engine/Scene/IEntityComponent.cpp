#include "IEntityComponent.hpp"

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