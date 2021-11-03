#pragma once

#include "../IEntityComponent.hpp"
#include <Math/Vec3.hpp>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

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

		btRigidBody* _rigidbody;
		btGhostObject* _trigger;
		btCollisionShape* _collision_shape;

		Vec3 _center;
		Vec3 _size;
		bool _is_trigger;

		btTransform GetEntityTransform();
		btCollisionShape* GetBtShape();
	public:
		ColliderComponent();
		~ColliderComponent();

		ColliderShape GetShape();

		void SetColliderShape(ColliderShape shape);

		const Vec3& GetSize();

		void SetSize(const Vec3& size);

		const Vec3& GetCenter();

		void SetCenter(const Vec3& center);

		void SetTrigger(bool trigger);

		bool IsTrigger();
		
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