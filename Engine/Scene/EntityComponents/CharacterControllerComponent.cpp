#include "CharacterControllerComponent.hpp"
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <bullet/LinearMath/btDefaultMotionState.h>
#include <Physics/PhysicsLayer.hpp>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

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

		if (!rayResult.m_collisionObject->hasContactResponse()) {
			return 1.0;
		}

		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}
private:
	btRigidBody* m_me;
};

class CharacterSweepCallback : public btCollisionWorld::ClosestConvexResultCallback {
public:
	const btCollisionObject* m_me;

	CharacterSweepCallback(const btCollisionObject* me, const btVector3& from, const btVector3& to)
		: ClosestConvexResultCallback(from, to), m_me(me) {
	}

	bool needsCollision(btBroadphaseProxy* proxy0) const override {

		if (!ClosestConvexResultCallback::needsCollision(proxy0)) {
			return false;
		}

		// »гнорируем собственный Rigidbody персонажа
		if (proxy0->m_clientObject == m_me) {
			return false;
		}

		const btCollisionObject* hitObj = static_cast<const btCollisionObject*>(proxy0->m_clientObject);

		if (!hitObj->hasContactResponse()) {
			return false;
		}

		return ClosestConvexResultCallback::needsCollision(proxy0);
	}
};

CharacterControllerComponent::CharacterControllerComponent() :
	_character(nullptr),
	_character_collision(nullptr),
	_motionState(nullptr),
	_is_on_ground(false),
	_size(1, 2),
	_skinWidth(0.02f)
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

float CharacterControllerComponent::GetSkinWidth() {
	return _skinWidth;
}
void CharacterControllerComponent::SetSkinWidth(float skinWidth) {
	_skinWidth = skinWidth;
}

bool CharacterControllerComponent::IsOnGround()
{
	float height = _size.y;
	
	btTransform currentTransform = _character->getWorldTransform();
	btVector3 current_pos = currentTransform.getOrigin();
	Vec3 pos = Vec3(current_pos.getX(), current_pos.getY(), current_pos.getZ());
	Vec3 target = pos + Vec3(0.f, -height - 0.05f, 0.f);
	
	ClosestNotMe result(_character);
	GetEntity()->GetScene()->GetPhysicalWorld()->RayTest(pos, target, result);

	return result.hasHit();
}

void CharacterControllerComponent::Move(const Vec3& displacement)
{
	const int maxIterations = 4;
	// Protection
	const float skinWidth = _skinWidth;
	btVector3 btDisplacement = btVector3(displacement.x, displacement.y, displacement.z);
	PhysicsWorld* world = GetEntity()->GetScene()->GetPhysicalWorld();

	for (int i = 0; i < maxIterations; ++i)
	{
		float distance = displacement.Length();
		if (distance < 0.0001f)
			break;

		btTransform from = _character->getWorldTransform();

		btVector3 direction = btDisplacement / distance;
		btVector3 start = from.getOrigin();
		// Calculating destination vector with skinWidth
		btVector3 end = start + direction * (distance + skinWidth);

		btTransform to = from;
		to.setOrigin(end);

		btTransform current = from;

		// Perform ConvexSweepTest itself
		CharacterSweepCallback callback(_character, start, end);
		world->ConvexSweepTest(_character_collision, from, to, callback);

		if (callback.hasHit()) 
		{
			// Safe distance to collision point
			float travelDist = callback.m_closestHitFraction * (distance + skinWidth) - skinWidth;
			travelDist = std::max(0.0f, travelDist);

			// Move character to that safe point
			current.setOrigin(start + direction * travelDist);
			_motionState->setWorldTransform(current);
			_character->setWorldTransform(current);
			// Update Entity position
			UpdateEntityPosition(current);

			// Remaining vector and its normal
			float remainingDist = distance - travelDist;
			btVector3 remainingVec = direction * remainingDist;
			btVector3 normal = callback.m_hitNormalWorld;

			// Projection: V_slide = V - (V Ј N) * N
			btDisplacement = remainingVec - normal * remainingVec.dot(normal);
		}
		else 
		{
			// Path is free Ч move by all remaining distance
			current.setOrigin(start + btDisplacement);
			_motionState->setWorldTransform(current);
			_character->setWorldTransform(current);
			// Update Entity position
			UpdateEntityPosition(current);
			break;
		}
	}
}

btTransform CharacterControllerComponent::GetEntityTransform() {
	btTransform result;

	Vec3 abs_pos = _entity->GetAbsolutePosition() + _center;
	Quat abs_rot = _entity->GetAbsoluteRotation();

	result.setOrigin(btVector3(abs_pos.x, abs_pos.y, abs_pos.z));
	result.setRotation(btQuaternion(abs_rot.x, abs_rot.y, abs_rot.z, abs_rot.w));

	return result;
}

void CharacterControllerComponent::UpdateEntityPosition(const btTransform& transform)
{
	btVector3 bullet_pos = transform.getOrigin();
	btQuaternion bullet_rot = transform.getRotation();

	Vec3 pos = Vec3(bullet_pos.getX(), bullet_pos.getY(), bullet_pos.getZ());
	pos = pos - _center;

	Quat rot = Quat(bullet_rot.getX(), bullet_rot.getY(), bullet_rot.getZ(), bullet_rot.getW());

	Mat4 mm = GetTransform(pos, GetEntity()->GetScale(), rot);
	Mat4 parent_tranform = GetEntity()->GetParent()->GetWorldTransform();
	Mat4 _new = mm * parent_tranform.invert();

	_entity->SetPosition(_new.GetPosition());
}

void CharacterControllerComponent::AddToWorld()
{
	btVector3 localInertia(0, 0, 0);

	// Kinematic preset
	float mass = 0;
	btVector3 gravity = btVector3(0, 0, 0);

	// release old rigidbody BEFORE collision shape
	if (_character)
	{
		// remove it from scene, first
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_character);
		// destroy object itself
		SAFE_RELEASE(_character);
	}

	// release old collider shape
	SAFE_RELEASE(_character_collision);
	// Release old motion state
	SAFE_RELEASE(_motionState)

	// Create collision shape
	float radius = _size.x;
	float cylinderHeight = _size.y - 2.0f * radius;
	_character_collision = new btCapsuleShape(radius, cylinderHeight);

	btTransform startTransform = GetEntityTransform();
	_motionState = new btDefaultMotionState(startTransform);

	// Info
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, _motionState, _character_collision, localInertia);
	constructionInfo.m_friction = 0;

	// Create rigidbody
	_character = new btRigidBody(constructionInfo);
	_character->setUserPointer(_entity);
	_character->setGravity(gravity);
	// Block rotation over X and Z
	_character->setAngularFactor(btVector3(0, 1, 0));

	int collisionFlags = _character->getCollisionFlags();
	collisionFlags |= btCollisionObject::CF_KINEMATIC_OBJECT;
	collisionFlags &= ~btCollisionObject::CF_STATIC_OBJECT;
	_character->setCollisionFlags(collisionFlags);
	_character->setActivationState(DISABLE_DEACTIVATION);

	// Add to world
	// Overriding flags, overwise kinematic objects won't work with triggers
	_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_character, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter);
}

void CharacterControllerComponent::OnUpdate() {
	if (!_character)
		AddToWorld();
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
	serializer.Serialize(_skinWidth);
}
void CharacterControllerComponent::Deserialize(ByteSolver& solver) {
	_size = solver.GetValue<Vec2>();
	_center = solver.GetValue<Vec3>();
	_skinWidth = solver.GetValue<float>();
}