#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {
	class ColliderComponent : public IEntityComponent {
	private:

	public:
		ColliderComponent() {

		}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_COLLIDER, "Collider")
	};
}