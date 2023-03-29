#include "ColliderComponent.hpp"
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <bullet/LinearMath/btDefaultMotionState.h>
#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btSphereShape.h>
#include <Core/YamlHelper.hpp>
#include <Physics/PhysicsLayer.hpp>

using namespace YAML;
using namespace VSGE;

ColliderComponent::ColliderComponent() :
	_shape(COLLIDER_SHAPE_CUBE),
	_rigidbody(nullptr),
	_trigger(nullptr),
	_collision_shape(nullptr),
	_size(1, 1, 1),
	_is_trigger(false)
{
}

ColliderComponent::~ColliderComponent() {

}

ColliderShape ColliderComponent::GetShape() {
	return _shape;
}

void ColliderComponent::SetColliderShape(ColliderShape shape) {
	_shape = shape;
}

const Vec3& ColliderComponent::GetSize() {
	return _size;
}

void ColliderComponent::SetSize(const Vec3& size) {
	_size = size;
}

const Vec3& ColliderComponent::GetCenter() {
	return _center;
}

void ColliderComponent::SetCenter(const Vec3& center) {
	_center = center;
}

void ColliderComponent::AddToWorld() {
	btVector3 local_intertia(0, 0, 0);

	if(_rigidbody)
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidbody);
	if (_trigger)
		_entity->GetScene()->GetPhysicalWorld()->RemoveCollisionObject(_trigger);

	//release old collider shape
	SAFE_RELEASE(_collision_shape);
	//release old rigidbody
	SAFE_RELEASE(_rigidbody);
	//release old ghost
	SAFE_RELEASE(_trigger);
	//Create new collision shape
	_collision_shape = GetBtShape();
	//check new shape
	if (_collision_shape == nullptr)
		//shape wasn't created, exiting
		return;

	btTransform startTransform = GetEntityTransform();
	if (!_is_trigger) {
		//using motionstate is recommended, itprovides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
		//rigidbody info
		btRigidBody::btRigidBodyConstructionInfo constructionInfo(0, motionState, _collision_shape, local_intertia);
		_rigidbody = new btRigidBody(constructionInfo);
		_rigidbody->setUserPointer(_entity);
		//apply gravity
		_rigidbody->setGravity(btVector3(0, 0, 0));
		//add rigidbody to world
		_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_rigidbody);
	}
	else {
		_trigger = new btGhostObject();
		_trigger->setCollisionFlags(_trigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		_trigger->setCollisionShape(_collision_shape);
		_trigger->setUserPointer(_entity);
		_trigger->setWorldTransform(startTransform);
		_entity->GetScene()->GetPhysicalWorld()->AddCollisionObject(_trigger);
	}
}

btCollisionShape* ColliderComponent::GetBtShape() {
	btCollisionShape* shape = nullptr;
	Vec3 scale = _entity->GetAbsoluteScale();

	switch (_shape) {
	case COLLIDER_SHAPE_CUBE:
		shape = new btBoxShape(btVector3(btScalar(scale.x),
		btScalar(scale.y),
		btScalar(scale.z)));
		break;
	case COLLIDER_SHAPE_SPHERE:
		shape = new btSphereShape(scale.y);
		break;
	case COLLIDER_SHAPE_MESH:
		break;
	}

	if (shape) {
		shape->setLocalScaling(btVector3(_size.x, _size.y, _size.z));
	}

	return shape;
}

btTransform ColliderComponent::GetEntityTransform() {
	btTransform result;

	Vec3 abs_pos = _entity->GetAbsolutePosition() + _center;
	Quat abs_rot = _entity->GetAbsoluteRotation();

	result.setOrigin(btVector3(abs_pos.x, abs_pos.y, abs_pos.z));
	result.setRotation(btQuaternion(abs_rot.x, abs_rot.y, abs_rot.z, abs_rot.w));

	return result;
}

void ColliderComponent::OnUpdate() {
	if (!_rigidbody && !_trigger)
		AddToWorld();

	if (_rigidbody) {
		btTransform transform = GetEntityTransform();
		_rigidbody->setWorldTransform(transform);
	}

	if (_trigger) {
		btTransform transform = GetEntityTransform();
		_trigger->setWorldTransform(transform);

		uint32 overlaping = _trigger->getNumOverlappingObjects();
		for (uint32 i = 0; i < overlaping; i++) {
			btCollisionObject* coll_obj = _trigger->getOverlappingObject(i);
			Entity* entity = (Entity*)coll_obj->getUserPointer();
			_entity->CallOnTriggerStay(entity);
		}
	}
}

void ColliderComponent::OnDestroy() {
	if (_rigidbody) {
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidbody);
		SAFE_RELEASE(_rigidbody)
	}
	if (_trigger) {
		_entity->GetScene()->GetPhysicalWorld()->RemoveCollisionObject(_trigger);
		SAFE_RELEASE(_trigger)
	}
	SAFE_RELEASE(_collision_shape)
}

void ColliderComponent::OnActivate() {
	if (_rigidbody) {
		_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_rigidbody);
	}
	if (_trigger) {
		_entity->GetScene()->GetPhysicalWorld()->AddCollisionObject(_trigger);
	}
}

void ColliderComponent::OnDeactivate() {
	if (_rigidbody) {
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidbody);
	}
	if (_trigger) {
		_entity->GetScene()->GetPhysicalWorld()->RemoveCollisionObject(_trigger);
	}
}

void ColliderComponent::SetTrigger(bool trigger) {
	if (_is_trigger != trigger) {
		_is_trigger = trigger;
		AddToWorld();
	}
}

bool ColliderComponent::IsTrigger() {
	return _is_trigger;
}

void ColliderComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_is_trigger);
	serializer.Serialize(_shape);
	serializer.Serialize(_center);
	serializer.Serialize(_size);
}
void ColliderComponent::Deserialize(ByteSolver& solver) {
	_is_trigger = solver.GetValue<bool>();
	_shape = solver.GetValue<ColliderShape>();
	_center = solver.GetValue<Vec3>();
	_size = solver.GetValue<Vec3>();
}