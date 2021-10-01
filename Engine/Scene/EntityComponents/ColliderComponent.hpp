#pragma once

#include "../IEntityComponent.hpp"
#include <Math/Vec3.hpp>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>

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

		btRigidBody* _rigidBody;
		btCollisionShape* _collision_shape;

		Vec3 _center;
		Vec3 _size;

		btTransform GetEntityTransform();
		btCollisionShape* GetBtShape();
	public:
		ColliderComponent();

		ColliderShape GetShape();

		void SetColliderShape(ColliderShape shape);

		const Vec3& GetSize();

		void SetSize(const Vec3& size);

		const Vec3& GetCenter();

		void SetCenter(const Vec3& center);
		
		void AddToWorld();

		void OnUpdate();
		void OnDestroy();

		void OnActivate();
		void OnDeactivate();

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_COLLIDER, "Collider")
	};
}