#pragma once

#include "../IEntityComponent.hpp"
#include <Math/Vec3.hpp>


#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>


namespace VSGE {

	class RigidBodyComponent : public IEntityComponent {
	private:
		float _mass;

		btRigidBody* _rigidBody;
	public:
		RigidBodyComponent();

		float GetMass();
		void SetMass(float mass);

		void ClearForces();
		void ApplyCentralImpulse(const Vec3& impulse);
		void ApplyCentralForce(const Vec3& force);

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_RIGIDBODY, "Rigid body")
	};
}