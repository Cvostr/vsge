#include "PhysicsWorld.hpp"
#include "PhysicsLayer.hpp"
#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

using namespace VSGE;

PhysicsWorld::PhysicsWorld() {
	_world = nullptr;
    _active = true;
}
PhysicsWorld::~PhysicsWorld() {
	Destroy();
}

void PhysicsWorld::Create() {
	PhysicsLayer* layer = PhysicsLayer::Get();
	_world = new btSoftRigidDynamicsWorld(
		layer->GetCollisionDispatcher(),
		layer->GetBroadphase(),
		layer->GetConstraintSolver(),
		layer->GetCollisionConfiguration());

    _world->getBroadphase()->getOverlappingPairCache()->
        setInternalGhostPairCallback(layer->GetGhostPairCallback());

    PhysicsLayer::Get()->AddWorld(this);
}
void PhysicsWorld::Destroy() {
    PhysicsLayer::Get()->RemoveWorld(this);
	SAFE_RELEASE(_world)
}
btSoftRigidDynamicsWorld* PhysicsWorld::GetWorld() {
	return _world;
}

void PhysicsWorld::StepSimulation(float timestep) {
    if(_active)
        _world->stepSimulation(timestep);
}

void PhysicsWorld::SetActive(bool active){
    _active = active;
}

void PhysicsWorld::AddRigidbody(btRigidBody* rigidbody) {
    if (!_world || !rigidbody)
        return;

    _world->addRigidBody(rigidbody);
}
void PhysicsWorld::RemoveRigidbody(btRigidBody* rigidbody) {
    if (!_world || !rigidbody)
        return;

    _world->removeRigidBody(rigidbody);
}
void PhysicsWorld::AddSoftBody(btSoftBody* softbody) {
    if (!_world || !softbody)
        return;

    _world->addSoftBody(softbody);
}
void PhysicsWorld::RemoveSoftBody(btSoftBody* softbody) {
    if (!_world || !softbody)
        return;

    _world->removeSoftBody(softbody);
}
void PhysicsWorld::AddCollisionObject(btCollisionObject* object) {
    if (!_world || !object)
        return;

    _world->addCollisionObject(object);
}
void PhysicsWorld::RemoveCollisionObject(btCollisionObject* object) {
    if (!_world || !object)
        return;

    _world->removeCollisionObject(object);
}

Entity* PhysicsWorld::RayTestFirstEntity(const Vec3& position, const Vec3& direction, float far) {
    Vec3 dir = direction.GetNormalized();
    btVector3 from = btVector3(position.x, position.y, position.z);
    btVector3 to = btVector3(dir.x * far, dir.y * far, dir.z * far) + from;

    btCollisionWorld::ClosestRayResultCallback  allResults(from, to);
    allResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    _world->rayTest(from, to, allResults);
    if (allResults.hasHit())
        return (Entity*)(allResults.m_collisionObject->getUserPointer());
    else
        return nullptr;
}

void PhysicsWorld::RayTest(const Vec3& position, const Vec3& direction, btCollisionWorld::RayResultCallback& callback) {
    btVector3 bvpos = btVector3(position.x, position.y, position.z);
    btVector3 bvdir = btVector3(direction.x, direction.y, direction.z);
    _world->rayTest(bvpos, bvdir, callback);
}