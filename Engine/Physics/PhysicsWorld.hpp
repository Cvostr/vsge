#pragma once

#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <Math/Vec3.hpp>
#include <Scene/Entity.hpp>

namespace VSGE {
	class PhysicsWorld {
	private:
		btSoftRigidDynamicsWorld* _world;
		//btSoftBodyWorldInfo* _world_info;
		bool _active;
	public:

		PhysicsWorld();
		~PhysicsWorld();

		void Create();
		void Destroy();

		btSoftRigidDynamicsWorld* GetWorld();
		/// <summary>
		/// Step physics simulation on this world
		/// </summary>
		/// <param name="timestep"></param>
		void StepSimulation(float timestep);
		/// <summary>
		/// Enable or disable physics simulation on this world
		/// </summary>
		/// <param name="active"></param>
		void SetActive(bool active);
		/// <summary>
		/// add created rigidbody to world
		/// </summary>
		void AddRigidbody(btRigidBody* rigidbody);
		/// <summary>
		/// remove rigidbody from world
		/// </summary>
		/// <param name="softbody"></param>
		void RemoveRigidbody(btRigidBody* rigidbody);
		/// <summary>
		/// add created softbody to world
		/// </summary>
		/// <param name="softbody">- created softbody</param>
		void AddSoftBody(btSoftBody* softbody);
		/// <summary>
		/// remove created softbody from world
		/// </summary>
		/// <param name="softbody">- created softbody</param>
		void RemoveSoftBody(btSoftBody* softbody);
		/// <summary>
		/// add created collision object to world
		/// </summary>
		/// <param name="object">- created object</param>
		void AddCollisionObject(btCollisionObject* object);
		/// <summary>
		/// remove created collision object from world
		/// </summary>
		/// <param name="object">- created object</param>
		void RemoveCollisionObject(btCollisionObject* object);

		Entity* RayTestFirstEntity(const Vec3& position, const Vec3& direction, float far);

		void RayTest(const Vec3& position, const Vec3& direction, btCollisionWorld::RayResultCallback& callback);

	};
}