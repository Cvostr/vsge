#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {

	enum ColliderShape {
		//COLLIDER_SHAPE_BOX,
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

		ColliderShape GetShape();

		void SetColliderShape(ColliderShape shape);

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_COLLIDER, "Collider")
	};
}