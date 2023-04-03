#pragma once

#include "../IEntityComponent.hpp"
#include <Math/Vec3.hpp>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>


namespace VSGE {

	class RigidBodyComponent : public IEntityComponent {
	private:
		float _mass;
		float _friction;
		float _rolling_friction;
		float _restitution;
		Vec3 _gravity;

		btRigidBody* _rigidBody;
		btCollisionShape* _collision_shape;
	public:
		RigidBodyComponent();

		float GetMass();
		void SetMass(float mass);

		void Activate();
		void Deactivate();
		void ClearForces();
		void ApplyCentralImpulse(const Vec3& impulse);
		void ApplyCentralForce(const Vec3& force);
		void SetLinearVelocity(const Vec3& velocity);
		void SetAngularVelocity(const Vec3& velocity);
		float GetFriction();
		void SetFriction(float friction);

		btTransform GetEntityTransform();
		btCollisionShape* CreateCollisionShape();

		void AddToWorld();

		void OnUpdate();
		void OnDestroy();

		void OnActivate();
		void OnDeactivate();

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_RIGIDBODY, "Rigid body")
	};
}