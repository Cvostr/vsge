#include "ColliderComponent.hpp"
#include <Scene/Entity.hpp>
#include <bullet/LinearMath/btDefaultMotionState.h>
#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btSphereShape.h>
#include <Core/YamlHelper.hpp>
#include <Physics/PhysicsLayer.hpp>

using namespace YAML;
using namespace VSGE;

ColliderComponent::ColliderComponent() :
	_shape(COLLIDER_SHAPE_CUBE),
	_rigidBody(nullptr),
	_collision_shape(nullptr),
	_size(1, 1, 1)
{
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

	//release old collider shape
	SAFE_RELEASE(_collision_shape);
	//release old rigidbody
	SAFE_RELEASE(_rigidBody);

	_collision_shape = GetBtShape();
	if (_collision_shape == nullptr)
		return;

	btTransform startTransform = GetEntityTransform();
	//using motionstate is recommended, itprovides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

	// Info
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(0, motionState, _collision_shape, local_intertia);

	_rigidBody = new btRigidBody(constructionInfo);
	_rigidBody->setUserPointer(_entity);

	_rigidBody->setGravity(btVector3(0, 0, 0));

	PhysicsLayer::Get()->AddRigidbody(_rigidBody);
}

btCollisionShape* ColliderComponent::GetBtShape() {
	btCollisionShape* shape = nullptr;
	Vec3 scale = _entity->GetAbsoluteScale();

	switch (_shape) {
	case COLLIDER_SHAPE_CUBE: {
		shape = new btBoxShape(btVector3(btScalar(scale.x),
		btScalar(scale.y),
		btScalar(scale.z)));
		break;
	}
	case COLLIDER_SHAPE_SPHERE: {
		shape = new btSphereShape(scale.y);
		break;
	}
	case COLLIDER_SHAPE_MESH: {
		break;
	}
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
	if (!_rigidBody)
		AddToWorld();

	if (_rigidBody) {
		btTransform transform = GetEntityTransform();
		_rigidBody->setWorldTransform(transform);
	}
}

void ColliderComponent::OnDestroy() {
	if (_rigidBody) {
		PhysicsLayer::Get()->RemoveRigidbody(_rigidBody);
		//delete _rigidBody;
	}
	SAFE_RELEASE(_collision_shape)
}

void ColliderComponent::Serialize(YAML::Emitter& e) {
	e << Key << "shape" << Value << _shape;
	e << Key << "center" << Value << _center;
	e << Key << "size" << Value << _size;
}

template<>
struct convert<Vec3>
{
	static Node encode(const Vec3& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, Vec3& rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}
};

void ColliderComponent::Deserialize(YAML::Node& entity) {
	_shape = (ColliderShape)entity["shape"].as<int>();
	_center = entity["center"].as<Vec3>();
	_size = entity["size"].as<Vec3>();
}
