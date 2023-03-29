#include "CharacterControllerComponent.hpp"
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <bullet/LinearMath/btDefaultMotionState.h>
#include <Physics/PhysicsLayer.hpp>
#include <Core/YamlHelper.hpp>
#include <Math/MatrixTransform.hpp>

using namespace YAML;
using namespace VSGE;

#define MASS 1.f
#define GRAVITY btVector3(0, -9.8f, 0)

CharacterControllerComponent::CharacterControllerComponent() :
	_character(nullptr),
	_character_collision(nullptr),
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
	return _is_on_ground;
}

void CharacterControllerComponent::Move(const Vec2& direction) {
	btVector3 current_vel_l = _character->getLinearVelocity();
	_character->setLinearVelocity(btVector3(direction.x, current_vel_l.y(), direction.y));
}

btTransform CharacterControllerComponent::GetEntityTransform() {
	btTransform result;

	Vec3 abs_pos = _entity->GetAbsolutePosition() + _center;
	Quat abs_rot = _entity->GetAbsoluteRotation();

	result.setOrigin(btVector3(abs_pos.x, abs_pos.y, abs_pos.z));
	result.setRotation(btQuaternion(abs_rot.x, abs_rot.y, abs_rot.z, abs_rot.w));

	return result;
}

void CharacterControllerComponent::AddToWorld() {
	btVector3 local_intertia(0, 0, 0);

	if (_character_collision && _character)
	{
		local_intertia = _character ? _character->getLocalInertia() : local_intertia;
		_character_collision->calculateLocalInertia(MASS, local_intertia);
	}

	//release old collider shape
	SAFE_RELEASE(_character_collision);
	//release old rigidbody
	SAFE_RELEASE(_character);

	_character_collision = new btCapsuleShape(_size.x, _size.y);

	if (!_character)
		_character_collision->calculateLocalInertia(MASS, local_intertia);

	btTransform startTransform = GetEntityTransform();

	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

	// Info
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(MASS, motionState, _character_collision, local_intertia);
	constructionInfo.m_mass = MASS;
	//constructionInfo.m_friction = _friction;
	//constructionInfo.m_rollingFriction = _rolling_friction;
	//constructionInfo.m_restitution = _restitution;
	constructionInfo.m_collisionShape = _character_collision;
	constructionInfo.m_localInertia = local_intertia;
	constructionInfo.m_motionState = motionState;

	_character = new btRigidBody(constructionInfo);
	_character->setUserPointer(_entity);
	_character->setGravity(GRAVITY);

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