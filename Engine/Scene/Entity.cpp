#include "Entity.hpp"
#include "Scene.hpp"
#include <algorithm>
#include "EntityComponents/MeshComponent.hpp"
#include "EntityComponents/AnimatorComponent.hpp"
#include <Math/MatrixTransform.hpp>
#include "SceneSerialization.hpp"
#include "string.h"

using namespace VSGE;

const Guid& Entity::GetGuid() const {
	return _id; 
}

void Entity::SetGuid(const Guid& id) { 
	_id = id; 
}

void Entity::SetScene(Scene* scene) {
	_scene = scene;
}

Scene* Entity::GetScene() const{
	return _scene;
}

void Entity::SetParent(Entity* parent) {
	_parent = parent; 
}

Entity* Entity::GetParent() const {
	return _parent; 
}

void Entity::SetActive(bool active) {
	_active = active; 
}

bool Entity::IsActive() const {
	if (!GetParent())
		return _active;
	return _active && GetParent()->IsActive();
}

void Entity::SetStatic(bool _static) {
	_static = _static; 
}

bool Entity::IsStatic() const {
	return _static; 
}

ViewMask Entity::GetViewMask() const {
	return _viewMask;
}

void Entity::SetViewMask(ViewMask viewMask) {
	_viewMask = viewMask;
}

uint32 Entity::GetChildrenCount() const {
	return static_cast<uint32>(_children.size()); 
}

uint32 Entity::GetTotalChildrenCount() const {
	uint32 result = GetChildrenCount();

	for (auto child : _children) {
		result += child->GetTotalChildrenCount();
	}

	return result;
}

void Entity::RemoveChild(Entity* entityToRemove) {
	if (entityToRemove) {
		if (HasChild(entityToRemove)) {
			tEntityList::iterator it = std::remove(_children.begin(), _children.end(), entityToRemove);
			_children.pop_back();
			entityToRemove->SetParent(nullptr);

			UpdateTransformMatrices();
			entityToRemove->UpdateTransformMatrices();

			Mat4 new_transform_mat = entityToRemove->GetLocalTransform() * _worldTransform;
			entityToRemove->SetPosition(new_transform_mat.GetPosition());
			entityToRemove->SetScale(new_transform_mat.GetScale());
			entityToRemove->SetRotation(GetRotationFromQuat(new_transform_mat));
		}
	}
}

void Entity::AddChild(Entity* entityToAdd, bool retransform) {
	if (entityToAdd) {
		if (!HasChild(entityToAdd)) {
			//Check, if child already has parent
			if (entityToAdd->GetParent()) {
				//make entity orphan
				entityToAdd->GetParent()->RemoveChild(entityToAdd);
			}
			//Add entity to new parent
			_children.push_back(entityToAdd);
			entityToAdd->SetParent(this);
			//retransform entity
			if (retransform) {
				UpdateTransformMatrices();
				entityToAdd->UpdateTransformMatrices();

				Mat4 new_transform_mat = entityToAdd->GetLocalTransform() * _worldTransform.invert();
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

Entity* Entity::GetEntityWithName(const std::string& name) {
	for (auto child : _children) {
		if (child->GetName() == name)
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
	_name = name;
}

const std::string& Entity::GetName() const {
	return _name; 
}

void Entity::Destroy() {
	//Remove child from entity's parent
	if (_parent) {
		_parent->RemoveChild(this);
	}
	for (auto component : _components) {
		component->OnDestroy();
	}
	//Call Destroy() on each child
	while(GetChildrenCount() > 0){
		_children[0]->Destroy();
	}
	//Destroy all entity components
	RemoveAllComponents();

	delete this;
}

uint32 Entity::GetComponentsCount() {
	return static_cast<uint32>(_components.size()); 
}

uint32 Entity::GetScriptsCount() {
	return static_cast<uint32>(_scripts.size());
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
	//mTransformDirty = true;
}
void Entity::SetScale(const Vec3& scale) {
	_scale = scale;
	//mTransformDirty = true;
}
void Entity::SetRotation(const Quat& rotation) {
	_rotation = rotation;
	//mTransformDirty = true;
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

	_boundingBox.ApplyTransform(_worldTransform);
	if (extendChildren) {
		for (auto child : _children) {
			child->GetAABB();
			_boundingBox.Extend(child->GetAABB());
		}
	}

	return _boundingBox;
}

const Mat4& Entity::GetWorldTransform() const
{ 
	return _worldTransform; 
}

void Entity::SetWorldTransform(const Mat4& transform) { 
	_worldTransform = transform;
}

void Entity::CallOnStart() {
	for (auto component : _components) {
		if(component->IsActive())
			component->OnStart();
	}

	for (auto script : _scripts) {
		if (script->IsActive())
			script->OnStart();
	}
}
void Entity::CallOnUpdate() {
	for (auto component : _components) {
		if (component->IsActive())
			component->OnUpdate();
	}

	for (auto script : _scripts) {
		if (script->IsActive())
			script->OnUpdate();
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
	return _worldTransform.GetPosition();
}
Vec3 Entity::GetAbsoluteScale() const {
	return _worldTransform.GetScale();
}
Quat Entity::GetAbsoluteRotation() const {
	return GetRotationFromQuat(_worldTransform);
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
}

Entity* Entity::GetRootSkinningEntity() {
	if (HasComponent<AnimatorComponent>())
		return this;

	return _parent->GetRootSkinningEntity();
}

Entity* Entity::GetChildEntityWithLabel(const std::string& label) {
	if (label == GetName()) {
		return this;
	}
	else {
		for (auto child : _children) {
			Entity* out = child->GetChildEntityWithLabel(label);
			if (out)
				return out;
		}
	}
	return nullptr;
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

	SceneSerializer serializer;
	serializer.SetScene(_scene);
	for (auto component : _components) {
		YAML::Emitter e;
		serializer.SerializeEntityComponent(component, e);
		
		YAML::Node data = YAML::Load(e.c_str());
		serializer.DeserializeEntityComponent(new_entity, data);

	}

	for (uint32 child_i = 0; child_i < _children.size(); child_i ++) {
		Entity* new_child = _children[child_i]->Dublicate();
		new_entity->AddChild(new_child);
	}

	return new_entity;
}

void Entity::ToPrefab(byte** out, uint32& size) {
	ByteSerialize serializer;
	//Write amount of objects
	uint32 objects_count = GetTotalChildrenCount() + 1;
	serializer.Serialize(objects_count);
	//Serialize objects
	SceneSerializer s_serializer;
	s_serializer.SetScene(GetScene());
	s_serializer.SerializeEntityBinary(this, serializer);
	//Copy results to buffer
	size = serializer.GetSerializedSize();
	*out = new byte[size];
	memcpy(*out, serializer.GetBytes(), size);
}