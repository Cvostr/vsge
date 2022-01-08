#include "PhysicsLayer.hpp"
#include <Core/VarTypes/Base.hpp>
#include <Core/Logger.hpp>
#include <Core/Time.hpp>
#include <Scene/SceneLayer.hpp>
#include <algorithm>

#include <bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

using namespace VSGE;

PhysicsLayer* PhysicsLayer::_this = nullptr;

PhysicsLayer::PhysicsLayer() {
    _this = this;

	_broadphase = new btDbvtBroadphase();
	_constraint_solver = new btSequentialImpulseConstraintSolver();

	_collision_configuration = new btSoftBodyRigidBodyCollisionConfiguration();
	_collision_dispatcher = new btCollisionDispatcher(_collision_configuration);

    _ghost_callback = new btGhostPairCallback();

    Logger::Log() << "Physics initialized\n";
}

PhysicsLayer::~PhysicsLayer() {
    SAFE_RELEASE(_ghost_callback);
    SAFE_RELEASE(_constraint_solver);
    SAFE_RELEASE(_collision_dispatcher);
    SAFE_RELEASE(_collision_configuration);
    SAFE_RELEASE(_broadphase);

    for (auto& world : _worlds) {
        SAFE_RELEASE(world);
    }
    _worlds.clear();
}

void PhysicsLayer::OnAttach() {

}
void PhysicsLayer::OnUpdate() {
    float delta_time = TimePerf::Get()->GetDeltaTime();
    for (auto& world : _worlds) {
        world->StepSimulation(delta_time);
    }
    //uint32 pre = TimePerf::Get()->GetTicks();
    //uint32 diff = TimePerf::Get()->GetTicks() - pre;
    //Logger::Log() << "DIFF " << diff;
}
void PhysicsLayer::OnDetach() {

}

btCollisionDispatcher* PhysicsLayer::GetCollisionDispatcher() {
    return _collision_dispatcher;
}
btBroadphaseInterface* PhysicsLayer::GetBroadphase() {
    return _broadphase;
}
btSequentialImpulseConstraintSolver* PhysicsLayer::GetConstraintSolver() {
    return _constraint_solver;
}
btDefaultCollisionConfiguration* PhysicsLayer::GetCollisionConfiguration() {
    return _collision_configuration;
}
btGhostPairCallback* PhysicsLayer::GetGhostPairCallback() {
    return _ghost_callback;
}
void PhysicsLayer::AddWorld(PhysicsWorld* world) {
    _worlds.push_back(world);
}
void PhysicsLayer::RemoveWorld(PhysicsWorld* world) {
    std::remove(_worlds.begin(), _worlds.end(), world);
    _worlds.pop_back();
}