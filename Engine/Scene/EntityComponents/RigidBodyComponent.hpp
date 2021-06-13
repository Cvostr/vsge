#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {

	class RigidBodyComponent : public IEntityComponent {
	private:
		float _mass;

	public:
		RigidBodyComponent() {
			_mass = 1.f;
		}

		float GetMass();
		void SetMass(float mass);

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_RIGIDBODY, "Rigid body")
	};
}