#include "PhysicsLayer.hpp"
#include <Core/VarTypes/Base.hpp>
#include <Core/Logger.hpp>
#include <Core/Time.hpp>

#include <bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>

using namespace VSGE;

PhysicsLayer::PhysicsLayer() {
	_broadphase = new btDbvtBroadphase();
	_constraint_solver = new btSequentialImpulseConstraintSolver();

	_collision_configuration = new btSoftBodyRigidBodyCollisionConfiguration();
	_collision_dispatcher = new btCollisionDispatcher(_collision_configuration);
	_world = new btSoftRigidDynamicsWorld(_collision_dispatcher, _broadphase, _constraint_solver, _collision_configuration);

    _world_info = new btSoftBodyWorldInfo();
    _world_info->m_sparsesdf.Initialize();
    _world->getDispatchInfo().m_enableSPU = true;
    _world_info->m_dispatcher = _collision_dispatcher;
    _world_info->m_broadphase = _broadphase;
    _world_info->air_density = (btScalar)1.2;
    _world_info->water_density = 0;
    _world_info->water_offset = 0;
    _world_info->water_normal = btVector3(0, 0, 0);
    _world_info->m_gravity = btVector3(_gravity.x, _gravity.y, _gravity.z);

    Logger::Log() << "Physics initialized\n";
}

PhysicsLayer::~PhysicsLayer() {
    SAFE_RELEASE(_world)
    SAFE_RELEASE(_constraint_solver)
    SAFE_RELEASE(_collision_dispatcher)
    SAFE_RELEASE(_collision_configuration)
    SAFE_RELEASE(_broadphase)
    SAFE_RELEASE(_world_info)
}

void PhysicsLayer::OnAttach() {

}
void PhysicsLayer::OnUpdate() {
    float delta_time = TimePerf::Get()->GetDeltaTime();
    _world->stepSimulation(delta_time);
}
void PhysicsLayer::OnDetach() {

}

void PhysicsLayer::AddRigidbody(btRigidBody* rigidbody) {
    if (!_world)
        return;

    _world->addRigidBody(rigidbody);
}
void PhysicsLayer::RemoveRigidbody(btRigidBody* rigidbody) {
    if (!_world)
        return;

    _world->removeRigidBody(rigidbody);
}