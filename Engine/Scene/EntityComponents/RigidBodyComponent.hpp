#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {

	class RigidBodyComponent : public IEntityComponent {
	private:
		float mass;

	public:
		RigidBodyComponent() {
			
		}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_RIGIDBODY, "Rigid body")
	};
}