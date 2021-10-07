#include "CharacterControllerComponent.hpp"
#include <Scene/Entity.hpp>
#include <bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <bullet/LinearMath/btDefaultMotionState.h>
#include <Physics/PhysicsLayer.hpp>

using namespace VSGE;

#define MASS 1.f

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

btTransform CharacterControllerComponent::GetEntityTransform() {
	btTransform result;

	Vec3 abs_pos = _entity->GetAbsolutePosition() + _center;
	Quat abs_rot = _entity->GetAbsoluteRotation();

	result.setOrigin(btVector3(abs_pos.x, abs_pos.y, abs_pos.z));
	result.setRotation(btQuaternion(abs_rot.x, abs_rot.y, abs_rot.z, abs_rot.w));

	return result;
}

void CharacterControllerComponent::OnUpdate() {
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
	//_character->setGravity(btVector3(_gravity.x, _gravity.y, _gravity.z));

	PhysicsLayer::Get()->AddRigidbody(_character);
}
void CharacterControllerComponent::OnDestroy() {

}

void CharacterControllerComponent::OnActivate() {

}
void CharacterControllerComponent::OnDeactivate() {

}

void CharacterControllerComponent::Serialize(YAML::Emitter& e) {

}
void CharacterControllerComponent::Deserialize(YAML::Node& entity) {

}

void CharacterControllerComponent::Serialize(ByteSerialize& serializer) {

}
void CharacterControllerComponent::Deserialize(ByteSolver& solver) {

}