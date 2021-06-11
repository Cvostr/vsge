#include "Entity.hpp"
#include "Scene.hpp"
#include <algorithm>
#include "EntityComponents/MeshComponent.hpp"
#include "EntityComponents/AnimatorComponent.hpp"
#include <Math/MatrixTransform.hpp>

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

			UpdateTransformMatrices();
			entityToRemove->UpdateTransformMatrices();

			Mat4 new_transform_mat = entityToRemove->GetLocalTransform() * WorldTransform;
			entityToRemove->SetPosition(new_transform_mat.GetPosition());
			entityToRemove->SetScale(new_transform_mat.GetScale());
			entityToRemove->SetRotation(GetRotationFromQuat(new_transform_mat));
		}
	}
}

void Entity::AddChild(Entity* entityToAdd, bool retransform) {
	if (entityToAdd) {
		if (!HasChild(entityToAdd)) {

			if (entityToAdd->GetParent()) {
				entityToAdd->GetParent()->RemoveChild(entityToAdd);
			}

			_children.push_back(entityToAdd);
			entityToAdd->SetParent(this);

			if (retransform) {
				UpdateTransformMatrices();
				entityToAdd->UpdateTransformMatrices();

				Mat4 new_transform_mat = entityToAdd->GetLocalTransform() * WorldTransform.invert();
				entityToAdd->SetPosition(new_transform_mat.GetPosition());
				entityToAdd->SetScale(new_transform_mat.GetScale());
				entityToAdd->SetRotation(GetRotationFromQuat(new_transform_mat));
			}
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

const AABB& Entity::GetAABB(bool extendChildren) {
	if (!GetParent())
		return _boundingBox;
	MeshComponent* mesh = GetComponent<MeshComponent>();
	Entity* parent = GetParent();

	if (mesh) {
		if (mesh->GetMeshResource()) {
			_boundingBox = mesh->GetMeshResource()->GetMesh()->GetBoundingBox();
		}
	}

	_boundingBox.ApplyTransform(WorldTransform);
	if (extendChildren) {
		for (auto child : _children) {
			child->GetAABB();
			_boundingBox.Extend(child->GetAABB());
		}
	}

	return _boundingBox;
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

Vec3 Entity::GetAbsolutePosition() const {
	return WorldTransform.GetPosition();
}
Vec3 Entity::GetAbsoluteScale() const {
	return WorldTransform.GetScale();
}
Quat Entity::GetAbsoluteRotation() {
	return GetRotationFromQuat(WorldTransform);
}

void Entity::SetRotationEuler(const Vec3& rotation) {
	Vec3 current = GetRotationEuler();
	if ((rotation - current).Length() < 0.5f)
		return;
	Quat q;
	q.CreateFromEulerAngles(rotation);
	SetRotation(q);
}

void Entity::UpdateTransformMatrices() {
	//if (!this->mTransformDirty)
	//	return;
	//Calculate local transformation matrix
	Mat4 localTransform = GetTransform(GetPosition(), GetScale(), GetRotation());
	//Set local transform matrix to entity
	SetLocalTransform(localTransform);

	Mat4 worldTransform = localTransform;
	if (GetParent()) {
		worldTransform = worldTransform * GetParent()->GetWorldTransform();
	}

	//Update entity absolute transform matrix
	SetWorldTransform(worldTransform);
	mTransformDirty = false;
}

Entity* Entity::GetRootSkinningEntity() {
	if (HasComponent<AnimatorComponent>())
		return this;

	return _parent->GetRootSkinningEntity();
}

Entity* Entity::GetChildEntityWithLabelStartsWith(const std::string& label, int& len) {
	Entity* result = nullptr;
	for (auto child : _children) {
		if (label._Starts_with(child->GetName()) && (len < child->GetName().size())) {
			result = child;
			len = child->GetName().size();
			int old_len = len;

			Entity* result1 = child->GetChildEntityWithLabelStartsWith(label, len);

			if (len > old_len)
				result = result1;
		}
		else {
			Entity* result = child->GetChildEntityWithLabelStartsWith(label, len);

			if (result)
				return result;
		}
	}
	return result;
}

Entity* Entity::Dublicate() {
	Entity* new_entity = _scene->AddNewEntity(GetName());

	new_entity->SetPosition(GetAbsolutePosition());
	new_entity->SetScale(GetAbsoluteScale());
	new_entity->SetRotation(GetAbsoluteRotation());

	GetParent()->AddChild(new_entity);

	new_entity->SetActive(_active);
	new_entity->SetStatic(_static);
	new_entity->SetViewMask(GetViewMask());

	for (uint32 child_i = 0; child_i < _children.size(); child_i ++) {
		Entity* new_child = _children[child_i]->Dublicate();
		new_entity->AddChild(new_child);
	}

	return new_entity;
}