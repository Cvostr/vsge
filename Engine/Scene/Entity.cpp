#include "Entity.hpp"
#include "Scene.hpp"
#include <algorithm>

using namespace VSGE;

void Entity::RemoveChild(Entity* entityToRemove) {
	if (entityToRemove) {
		if (HasChild(entityToRemove)) {
			tEntityList::iterator it = std::remove(mChildren.begin(), mChildren.end(), entityToRemove);
			mChildren.pop_back();
			entityToRemove->SetParent(nullptr);
		}
	}
}

void Entity::AddChild(Entity* entityToAdd) {
	if (entityToAdd) {
		if (!HasChild(entityToAdd)) {

			if (entityToAdd->GetParent()) {
				entityToAdd->GetParent()->RemoveChild(entityToAdd);
			}

			mChildren.push_back(entityToAdd);
			entityToAdd->SetParent(this);
		}
	}
}

bool Entity::HasChild(Entity* entity) {
	for (auto child : mChildren) {
		if (entity == child)
			return true;
	}
	return false;
}

Entity* Entity::GetEntityWithName(std::string name) {
	for (auto child : mChildren) {
		if (child->GetName().compare(name) == 0)
			return child;
		Entity* result_from_child = child->GetEntityWithName(name);
		if (result_from_child)
			return result_from_child;
	}
	return nullptr;
}

Entity* Entity::GetEntityWithGuid(const Guid& id) {
	for (auto child : mChildren) {
		Guid g = child->GetGuid();
		if (g == id)
			return child;
		Entity* result_from_child = child->GetEntityWithGuid(id);
		if (result_from_child)
			return result_from_child;
	}
	return nullptr;
}

void Entity::SetName(const std::string& name) {
	int addition = 0;
	std::string name_to_test = name;
	Entity* ent = nullptr;
	while ((ent = mScene->GetEntityWithName(name_to_test)) && ent != this) {
		addition++;
		name_to_test = name + " (" + std::to_string(addition) + ")";
	}
	mName = name_to_test;
}

void Entity::Destroy() {
	//Remove child from entity's parent
	if (mParent) {
		mParent->RemoveChild(this);
	}
	//Call Destroy() on each child
	while(GetChildrenCount() > 0){
		mChildren[0]->Destroy();
	}
	//Destroy all entity components
	RemoveAllComponents();

	delete this;
}

bool Entity::HasComponent(IEntityComponent* component) {
	for (auto _component : mComponents) {
		if (component == _component)
			return true;
	}
	return false;
}

void Entity::RemoveComponent(IEntityComponent* component) {
	if (component) {
		if (HasComponent(component)) {
			tComponentList::iterator it = std::remove(mComponents.begin(), mComponents.end(), component);
			mComponents.pop_back();
			component->OnDestroy();
		}
	}
}

void Entity::RemoveAllComponents() {
	while(GetComponentsCount() > 0){
		RemoveComponent(mComponents[0]);
	}
}

void Entity::SetPosition(const Vec3& position) {
	mPosition = position;
	mTransformDirty = true;
}
void Entity::SetScale(const Vec3& scale) {
	mScale = scale;
	mTransformDirty = true;
}
void Entity::SetRotation(const Vec3& rotation) {
	mRotation = rotation;
	mTransformDirty = true;
}
