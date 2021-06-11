#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {

	enum ColliderShape {
		COLLIDER_SHAPE_BOX,
		COLLIDER_SHAPE_CUBE,
		COLLIDER_SHAPE_SPHERE,
		COLLIDER_SHAPE_MESH
	};

	class ColliderComponent : public IEntityComponent {
	private:
		ColliderShape _shape;
	public:
		ColliderComponent() :
			_shape(COLLIDER_SHAPE_CUBE)
		{
		}

		ColliderShape GetShape() {
			return _shape;
		}

		void SetColliderShape(ColliderShape shape) {
			_shape = shape;
		}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_COLLIDER, "Collider")
	};
}