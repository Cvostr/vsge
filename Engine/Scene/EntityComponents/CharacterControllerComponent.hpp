#pragma once

#include "../IEntityComponent.hpp"
#include <Math/Vec3.hpp>
#include <Math/Vec2.hpp>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

namespace VSGE {

	class CharacterControllerComponent : public IEntityComponent {
	private:
		btCollisionShape* _character_collision;
		btRigidBody* _character;
		Vec2 _size;
		Vec3 _center;
		bool _is_on_ground;

		btTransform GetEntityTransform();
	public:
		CharacterControllerComponent();

		const Vec2& GetSize();
		void SetSize(const Vec2& size);

		const Vec3& GetCenter();
		void SetCenter(const Vec3& center);

		bool IsOnGround();

		void OnUpdate();
		void OnDestroy();

		void OnActivate();
		void OnDeactivate();

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_CHARACTER_CONTROLLER, "CharacterController")
	};
}