#pragma once

#include "../Engine/ApplicationLayer.hpp"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBodyInternals.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include "PhysicsWorld.hpp"

namespace VSGE {
	class PhysicsLayer : public IApplicationLayer {
	private:
		btBroadphaseInterface* _broadphase;
		btCollisionDispatcher* _collision_dispatcher;
		btSequentialImpulseConstraintSolver* _constraint_solver;
		btDefaultCollisionConfiguration* _collision_configuration;
		btGhostPairCallback* _ghost_callback;

		static PhysicsLayer* _this;

		std::vector<PhysicsWorld*> _worlds;
	public:

		PhysicsLayer();
		~PhysicsLayer();

		static PhysicsLayer* Get() {
			return _this;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		btCollisionDispatcher* GetCollisionDispatcher();
		btBroadphaseInterface* GetBroadphase();
		btSequentialImpulseConstraintSolver* GetConstraintSolver();
		btDefaultCollisionConfiguration* GetCollisionConfiguration();
		btGhostPairCallback* GetGhostPairCallback();

		void AddWorld(PhysicsWorld* world);
		void RemoveWorld(PhysicsWorld* world);
	};
}