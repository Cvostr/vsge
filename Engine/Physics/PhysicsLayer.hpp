#pragma once

#include "../Engine/ApplicationLayer.hpp"
#include <Math/Vec3.hpp>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBodyInternals.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

namespace VSGE {
	class PhysicsLayer : public IApplicationLayer {
	private:
		btBroadphaseInterface* _broadphase;
		btCollisionDispatcher* _collision_dispatcher;
		btSequentialImpulseConstraintSolver* _constraint_solver;
		btDefaultCollisionConfiguration* _collision_configuration;
		btSoftRigidDynamicsWorld* _world;
		btSoftBodyWorldInfo* _world_info;
		btGhostPairCallback* _ghost_callback;

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
		/// <summary>
		/// add created rigidbody to world
		/// </summary>
		void AddRigidbody(btRigidBody* rigidbody);
		/// <summary>
		/// remove rigidbody from world
		/// </summary>
		/// <param name="softbody"></param>
		void RemoveRigidbody(btRigidBody* rigidbody);

		void AddSoftBody(btSoftBody* softbody);
		void RemoveSoftBody(btSoftBody* softbody);

		void AddCollisionObject(btCollisionObject* object);
		void RemoveCollisionObject(btCollisionObject* object);
	};
}