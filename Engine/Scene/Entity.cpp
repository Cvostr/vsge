#include "Entity.hpp"
#include "Scene.hpp"
#include <algorithm>
#include "EntityComponents/MeshComponent.hpp"

using namespace VSGE;

bool Entity::IsActive() const {
	if (!GetParent())
		return _active;
	return _active && GetParent()->IsActive();
}

void Entity::RemoveChild(Entity* entityToRemove) {
	if (entityToRemove) {
		if (HasChild(entityToRemove)) {
			tEntityList::iterator it = std::remove(_children.begin(), _children.end(), entityToRemove);
			_children.pop_back();
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

			_children.push_back(entityToAdd);
			entityToAdd->SetParent(this);
		}
	}
}

bool Entity::HasChild(Entity* entity) {
	for (auto child : _children) {
		if (entity == child)
			return true;
	}
	return false;
}

Entity* Entity::GetEntityWithName(std::string name) {
	for (auto child : _children) {
		if (child->GetName().compare(name) == 0)
			return child;
		Entity* result_from_child = child->GetEntityWithName(name);
		if (result_from_child)
			return result_from_child;
	}
	return nullptr;
}

Entity* Entity::GetEntityWithGuid(const Guid& id) {
	if (_id == id)
		return this;
	for (auto child : _children) {
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
	while ((ent = _scene->GetEntityWithName(name_to_test)) && ent != this) {
		addition++;
		name_to_test = name + " (" + std::to_string(addition) + ")";
	}
	_name = name_to_test;
}

void Entity::Destroy() {
	//Remove child from entity's parent
	if (_parent) {
		_parent->RemoveChild(this);
	}
	//Call Destroy() on each child
	while(GetChildrenCount() > 0){
		_children[0]->Destroy();
	}
	//Destroy all entity components
	RemoveAllComponents();

	delete this;
}

void Entity::AddComponent(IEntityComponent* component) {
	component->SetEntity(this);
	_components.push_back(component);
}

bool Entity::HasComponent(IEntityComponent* component) {
	for (auto _component : _components) {
		if (component == _component)
			return true;
	}
	return false;
}

void Entity::RemoveComponent(IEntityComponent* component) {
	if (component) {
		if (HasComponent(component)) {
			tComponentList::iterator it = std::remove(_components.begin(), _components.end(), component);
			_components.pop_back();
			component->OnDestroy();
		}
	}
}

void Entity::RemoveAllComponents() {
	while(GetComponentsCount() > 0){
		RemoveComponent(_components[0]);
	}
}

void Entity::SetPosition(const Vec3& position) {
	_position = position;
	mTransformDirty = true;
}
void Entity::SetScale(const Vec3& scale) {
	_scale = scale;
	mTransformDirty = true;
}
void Entity::SetRotation(const Quat& rotation) {
	_rotation = rotation;
	mTransformDirty = true;
}

const AABB& Entity::UpdateAABB() {
	if (!GetParent())
		return mBoundingBox;
	MeshComponent* mesh = GetComponent<MeshComponent>();
	Entity* parent = GetParent();

	if (mesh) {
		if (mesh->GetMeshResource()) {
			mBoundingBox = mesh->GetMeshResource()->GetMesh()->GetBoundingBox();
		}
	}

	mBoundingBox.ApplyTransform(LocalTransform);

	for (auto child : _children) {
		child->UpdateAABB();
		mBoundingBox.Extend(child->UpdateAABB());
	}

	return mBoundingBox;
}

void Entity::SetWorldTransform(const Mat4& transform) { 
	WorldTransform = transform;
}

void Entity::CallOnStart() {
	for (auto component : _components) {
		if(component->IsActive())
			component->OnStart();
	}
}
void Entity::CallOnUpdate() {
	for (auto component : _components) {
		if (component->IsActive())
			component->OnUpdate();
	}
}
void Entity::CallOnStop() {
	for (auto component : _components) {
		if (component->IsActive())
			component->OnDestroy();
	}
}
void Entity::CallOnPreRender() {
	for (auto component : _components) {
		if (component->IsActive())
			component->OnPreRender();
	}
}

Vec3 Entity::GetAbsolutePosition() {
	return WorldTransform.GetPosition();
}
Vec3 Entity::GetAbsoluteScale() {
	return WorldTransform.GetScale();
}
Vec3 Entity::GetAbsoluteRotation() {
	return WorldTransform.GetRotation();
}