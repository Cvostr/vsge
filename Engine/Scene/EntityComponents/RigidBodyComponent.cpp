#include "RigidBodyComponent.hpp"
#include "MeshComponent.hpp"
#include <bullet/LinearMath/btDefaultMotionState.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h>
#include <Physics/PhysicsLayer.hpp>
#include <Math/MatrixTransform.hpp>
#include "../Entity.hpp"
#include <Scene/Scene.hpp>

using namespace VSGE;

RigidBodyComponent::RigidBodyComponent():
	_mass(1.f),
	_friction(0.5f),
	_rolling_friction(0.0f),
	_restitution(0.0f),
	_gravity(0, -9.8f, 0),

	_rigidBody(nullptr),
	_collision_shape(nullptr)
{
}

float RigidBodyComponent::GetMass() {
	return _mass;
}

void RigidBodyComponent::SetMass(float mass) {
	if (mass >= 0 && _mass != mass) {
		_mass = mass;

		if (_rigidBody) {
			_rigidBody->setMassProps(mass, _rigidBody->getLocalInertia());
		}
	}
}

void RigidBodyComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_mass);
	serializer.Serialize(_friction);
}

void RigidBodyComponent::Deserialize(ByteSolver& solver) {
	_mass = solver.GetValue<float>();
	_friction = solver.GetValue<float>();
}

void RigidBodyComponent::Activate() {
	if (!_rigidBody)
		return;

	_rigidBody->activate(true);
}

void RigidBodyComponent::Deactivate() {
	if (!_rigidBody)
		return;

	_rigidBody->setActivationState(WANTS_DEACTIVATION);
}

void RigidBodyComponent::ClearForces() {
	if (!_rigidBody)
		return;

	_rigidBody->clearForces();
}

void RigidBodyComponent::ApplyCentralImpulse(const Vec3& impulse) {
	if (!_rigidBody) 
		return;

	_rigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
}
void RigidBodyComponent::ApplyCentralForce(const Vec3& force) {
	if (!_rigidBody) 
		return;

	_rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void RigidBodyComponent::SetLinearVelocity(const Vec3& velocity) {
	if (!_rigidBody)
		return;

	_rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

void RigidBodyComponent::SetAngularVelocity(const Vec3& velocity) {
	if (!_rigidBody)
		return;

	_rigidBody->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

float RigidBodyComponent::GetFriction() {
	return _friction;
}

void RigidBodyComponent::SetFriction(float friction) {
	_friction = friction;

	if (_rigidBody) {
		_rigidBody->setFriction(_friction);
	}
}

btTransform RigidBodyComponent::GetEntityTransform() {
	btTransform result;

	Vec3 abs_pos = _entity->GetAbsolutePosition();
	Quat abs_rot = _entity->GetAbsoluteRotation();

	result.setOrigin(btVector3(abs_pos.x, abs_pos.y, abs_pos.z));
	result.setRotation(btQuaternion(abs_rot.x, abs_rot.y, abs_rot.z, abs_rot.w));

	return result;
}

btCollisionShape* RigidBodyComponent::CreateCollisionShape() 
{
	Vec3 scale = _entity->GetAbsoluteScale();

	//�������� ������ MeshComponent
	MeshComponent* meshComponent = _entity->GetComponent<MeshComponent>();

	if (meshComponent) 
	{
		//������ ������� ��������, �������� ������ Mesh
		MeshResource* meshResource = meshComponent->GetResourceReference().GetResource<MeshResource>();
		if (meshResource) 
		{
			//������� �������� ������
			//��������� ���������
			if (!meshResource->IsReady()) 
			{
				//������ �� ��������, ������ � ������� �� �������� � ������� � nullptr
				meshResource->Load();
				return nullptr;
			}

			Mesh* mesh = meshResource->GetMesh();
			Vec3* pos = mesh->GetPositions();

			btCollisionShape* shape = new btConvexHullShape((float*)pos, mesh->GetVerticesCount(), sizeof(Vec3));
			shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
			return shape;
		}
	}

	return nullptr;
}

void RigidBodyComponent::AddToWorld() 
{
	btVector3 local_intertia(0, 0, 0);

	if (_collision_shape && _rigidBody)
	{
		local_intertia = _rigidBody ? _rigidBody->getLocalInertia() : local_intertia;
		_collision_shape->calculateLocalInertia(_mass, local_intertia);
	}

	//release old collider shape
	SAFE_RELEASE(_collision_shape);
	//release old rigidbody
	SAFE_RELEASE(_rigidBody);

	_collision_shape = CreateCollisionShape();
	if (_collision_shape == nullptr)
		return;

	if(!_rigidBody)
		_collision_shape->calculateLocalInertia(_mass, local_intertia);

	btTransform startTransform = GetEntityTransform();
	//using motionstate is recommended, itprovides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

	// Info
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(_mass, motionState, _collision_shape, local_intertia);
	constructionInfo.m_mass = _mass;
	constructionInfo.m_friction = _friction;
	constructionInfo.m_rollingFriction = _rolling_friction;
	constructionInfo.m_restitution = _restitution;
	constructionInfo.m_collisionShape = _collision_shape;
	constructionInfo.m_localInertia = local_intertia;
	constructionInfo.m_motionState = motionState;

	_rigidBody = new btRigidBody(constructionInfo);
	_rigidBody->setUserPointer(_entity);
	_rigidBody->setGravity(btVector3(_gravity.x, _gravity.y, _gravity.z));

	_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_rigidBody);
}

void RigidBodyComponent::OnUpdate() 
{
	//��������, ������ �� ������ rigidBody
	if (!_rigidBody) 
	{
		//���� �� ������, ��������� �������
		AddToWorld();
	}

	if (!_rigidBody)
	{
		//������� �� ����������, �������
		return;
	}

	//���������� ������ ������������� �� bullet � ��������� ��������� ������� � ����
	btVector3 bullet_pos = _rigidBody->getCenterOfMassPosition();
	btQuaternion bullet_rot = _rigidBody->getOrientation();

	Vec3 pos = Vec3(bullet_pos.getX(), bullet_pos.getY(), bullet_pos.getZ());
	Quat rot = Quat(bullet_rot.getX(), bullet_rot.getY(), bullet_rot.getZ(), bullet_rot.getW());

	//�������� ������� ���������� �������������
	Mat4 entityAbsoluteTransformMat = GetTransform(pos, GetEntity()->GetScale(), rot);
	//��������� ������� ������������� ��������
	Mat4 parent_tranform = GetEntity()->GetParent()->GetWorldTransform();
	//��������� ������� ��������� �������������
	Mat4 entityLocalTransformMat = entityAbsoluteTransformMat * parent_tranform.invert();

	//���������� ��������� ������������� ��� �������
	_entity->SetPosition(entityLocalTransformMat.GetPosition());
	_entity->SetRotation(GetRotationFromQuat(entityLocalTransformMat));

	Activate();
}

void RigidBodyComponent::OnDestroy() 
{
	if (_rigidBody) 
	{
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidBody);
		SAFE_RELEASE(_rigidBody)
	}
	SAFE_RELEASE(_collision_shape)
}

void RigidBodyComponent::OnActivate() 
{
	if (_rigidBody)
	{
		_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_rigidBody);
	}
}

void RigidBodyComponent::OnDeactivate() 
{
	if (_rigidBody)
	{
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidBody);
	}
}