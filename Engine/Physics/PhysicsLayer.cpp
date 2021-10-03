#include "PhysicsLayer.hpp"
#include <Core/VarTypes/Base.hpp>
#include <Core/Logger.hpp>
#include <Core/Time.hpp>
#include <Scene/SceneLayer.hpp>

#include <bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>

using namespace VSGE;

PhysicsLayer* PhysicsLayer::_this = nullptr;

PhysicsLayer::PhysicsLayer() {
    _this = this;

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

    _ghost_callback = new btGhostPairCallback();
    _world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(_ghost_callback);

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
    if (SceneLayer::Get()->IsSceneRunning()) {
        float delta_time = TimePerf::Get()->GetDeltaTime();
        _world->stepSimulation(delta_time);
    }
}
void PhysicsLayer::OnDetach() {

}

void PhysicsLayer::AddRigidbody(btRigidBody* rigidbody) {
    if (!_world || !rigidbody)
        return;

    _world->addRigidBody(rigidbody);
}
void PhysicsLayer::RemoveRigidbody(btRigidBody* rigidbody) {
    if (!_world || !rigidbody)
        return;

    _world->removeRigidBody(rigidbody);
}
void PhysicsLayer::AddSoftBody(btSoftBody* softbody) {
    if (!_world || !softbody)
        return;

    _world->addSoftBody(softbody);
}
void PhysicsLayer::RemoveSoftBody(btSoftBody* softbody) {
    if (!_world || !softbody)
        return;

    _world->removeSoftBody(softbody);
}
void PhysicsLayer::AddCollisionObject(btCollisionObject* object) {
    if (!_world || !object)
        return;

    _world->addCollisionObject(object);
}
void PhysicsLayer::RemoveCollisionObject(btCollisionObject* object) {
    if (!_world || !object)
        return;

    _world->removeCollisionObject(object);
}