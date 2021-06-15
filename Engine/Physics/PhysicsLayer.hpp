#pragma once

#include "../Engine/ApplicationLayer.hpp"
#include <Math/Vec3.hpp>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBodyInternals.h>

namespace VSGE {
	class PhysicsLayer : public IApplicationLayer {
	private:
		btBroadphaseInterface* _broadphase;
		btCollisionDispatcher* _collision_dispatcher;
		btSequentialImpulseConstraintSolver* _constraint_solver;
		btDefaultCollisionConfiguration* _collision_configuration;
		btDiscreteDynamicsWorld* _world;
		btSoftBodyWorldInfo* _world_info;

		Vec3 _gravity;

		static PhysicsLayer* _this;
	public:

		PhysicsLayer();
		~PhysicsLayer();

		static PhysicsLayer* Get() {
			return _this;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		void AddRigidbody(btRigidBody* rigidbody);
		void RemoveRigidbody(btRigidBody* rigidbody);
	};
}