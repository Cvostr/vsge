#include "CharacterControllerComponent.hpp"
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <bullet/LinearMath/btDefaultMotionState.h>
#include <Physics/PhysicsLayer.hpp>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

#define MASS 1.f
#define GRAVITY btVector3(0, -9.8f, 0)

class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback
{
public:
	ClosestNotMe(btRigidBody* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_me = me;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == m_me)
			return 1.0;

		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}
private:
	btRigidBody* m_me;
};

CharacterControllerComponent::CharacterControllerComponent() :
	_character(nullptr),
	_character_collision(nullptr),
	_motionState(nullptr),
	_is_on_ground(false),
	_size(1, 2)
{

}

const Vec2& CharacterControllerComponent::GetSize() {
	return _size;
}
void CharacterControllerComponent::SetSize(const Vec2& size) {
	_size = size;
}

const Vec3& CharacterControllerComponent::GetCenter() {
	return _center;
}
void CharacterControllerComponent::SetCenter(const Vec3& center) {
	_center = center;
}

bool CharacterControllerComponent::IsOnGround() {
	float height = _size.y;
	btVector3 current_pos = _character->getCenterOfMassPosition();
	Vec3 pos = Vec3(current_pos.getX(), current_pos.getY(), current_pos.getZ());
	Vec3 target = pos + Vec3(0.f, (-height) * 1.12f, 0.f);

	ClosestNotMe result(_character);
	GetEntity()->GetScene()->GetPhysicalWorld()->RayTest(pos, target, result);

	return result.hasHit();
}

void CharacterControllerComponent::Move(const Vec3& direction) {
	btVector3 current_vel_l = _character->getLinearVelocity();
	_character->setLinearVelocity(btVector3(direction.x, direction.y, direction.z));
}

btTransform CharacterControllerComponent::GetEntityTransform() {
	btTransform result;

	Vec3 abs_pos = _entity->GetAbsolutePosition() + _center;
	Quat abs_rot = _entity->GetAbsoluteRotation();

	result.setOrigin(btVector3(abs_pos.x, abs_pos.y, abs_pos.z));
	result.setRotation(btQuaternion(abs_rot.x, abs_rot.y, abs_rot.z, abs_rot.w));

	return result;
}

void CharacterControllerComponent::AddToWorld() 
{
	btVector3 prevLocalInertia(0, 0, 0);
	btVector3 localInertia(0, 0, 0);

	// TODO: to settings?
	bool isKinematic = false;

	// Kinematic preset
	float mass = 0;
	btVector3 gravity = btVector3(0, 0, 0);

	// Save previous localInertia
	if (_character_collision && _character) {
		prevLocalInertia = _character->getLocalInertia();
	}

	// release old collider shape
	SAFE_RELEASE(_character_collision);
	// release old rigidbody
	SAFE_RELEASE(_character);
	// Release old motion state
	SAFE_RELEASE(_motionState)

	// Create collision shape
	_character_collision = new btCapsuleShape(_size.x, _size.y);

	// For non-kinematic mode, currently unused.
	// TODO: implement???
	if (!_character && isKinematic == false)
	{
		mass = MASS;
		gravity = GRAVITY;
		_character_collision->calculateLocalInertia(mass, localInertia);
	}

	btTransform startTransform = GetEntityTransform();
	_motionState = new btDefaultMotionState(startTransform);

	// Info
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, _motionState, _character_collision, localInertia);
	constructionInfo.m_friction = 0;
	//constructionInfo.m_rollingFriction = _rolling_friction;
	//constructionInfo.m_restitution = _restitution;

	// Create rigidbody
	_character = new btRigidBody(constructionInfo);
	_character->setUserPointer(_entity);
	_character->setGravity(gravity);
	// Block rotation over X and Z for non-kinematic mode
	if (!isKinematic)
		_character->setAngularFactor(btVector3(0, 1, 0));

	int collisionFlags = _character->getCollisionFlags();
	if (isKinematic)
		collisionFlags |= btCollisionObject::CF_KINEMATIC_OBJECT;
	_character->setCollisionFlags(collisionFlags);

	// add to world
	_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_character);
}

void CharacterControllerComponent::OnUpdate() {
	if (!_character)
		AddToWorld();

	btVector3 bullet_pos = _character->getCenterOfMassPosition();
	btQuaternion bullet_rot = _character->getOrientation();

	Vec3 pos = Vec3(bullet_pos.getX(), bullet_pos.getY(), bullet_pos.getZ());
	Quat rot = Quat(bullet_rot.getX(), bullet_rot.getY(), bullet_rot.getZ(), bullet_rot.getW());

	Mat4 mm = GetTransform(pos, GetEntity()->GetScale(), rot);
	Mat4 parent_tranform = GetEntity()->GetParent()->GetWorldTransform();
	Mat4 _new = mm * parent_tranform.invert();

	_entity->SetPosition(_new.GetPosition());
	_entity->SetRotation(GetRotationFromQuat(_new));

	Activate();
}
void CharacterControllerComponent::OnDestroy() {
	if (_character) {
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_character);
		SAFE_RELEASE(_character)
	}
	SAFE_RELEASE(_character_collision)
	// Release old motion state
	SAFE_RELEASE(_motionState)
}

void CharacterControllerComponent::Activate() {
	if (!_character)
		return;

	_character->activate(true);
}

void CharacterControllerComponent::Deactivate() {
	if (!_character)
		return;

	_character->setActivationState(WANTS_DEACTIVATION);
}

void CharacterControllerComponent::OnActivate() {
	if (_character) {
		_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_character);
	}
}
void CharacterControllerComponent::OnDeactivate() {
	if (_character) {
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_character);
	}
}

void CharacterControllerComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_size);
	serializer.Serialize(_center);
}
void CharacterControllerComponent::Deserialize(ByteSolver& solver) {
	_size = solver.GetValue<Vec2>();
	_center = solver.GetValue<Vec3>();
}