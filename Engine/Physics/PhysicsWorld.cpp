#include "PhysicsWorld.hpp"
#include "PhysicsLayer.hpp"
#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include <bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>

using namespace VSGE;

PhysicsWorld::PhysicsWorld():
    _world(nullptr),
    _constraint_solver(nullptr),
    _broadphase(nullptr),
    _collision_dispatcher(nullptr),
    _active(false)
{
}

PhysicsWorld::~PhysicsWorld() {
	Destroy();
}

void PhysicsWorld::Create() 
{
    PhysicsLayer* layer = PhysicsLayer::Get();

    // Setup World physics caches
    _broadphase = new btDbvtBroadphase();
    _constraint_solver = new btSequentialImpulseConstraintSolver();
    _collision_dispatcher = new btCollisionDispatcher(layer->GetCollisionConfiguration());

    // Create bullet physics world
	_world = new btSoftRigidDynamicsWorld(
        _collision_dispatcher,
        _broadphase,
        _constraint_solver,
        layer->GetCollisionConfiguration());

    _world->getBroadphase()->getOverlappingPairCache()->
        setInternalGhostPairCallback(layer->GetGhostPairCallback());

    PhysicsLayer::Get()->AddWorld(this);
}
void PhysicsWorld::Destroy() {
    PhysicsLayer::Get()->RemoveWorld(this);
	SAFE_RELEASE(_world)

    SAFE_RELEASE(_constraint_solver)
    SAFE_RELEASE(_broadphase)
    SAFE_RELEASE(_collision_dispatcher)
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

void PhysicsWorld::RayTest(const Vec3& position, const Vec3& direction, btCollisionWorld::RayResultCallback& callback) 
{
    btVector3 bvpos = btVector3(position.x, position.y, position.z);
    btVector3 bvdir = btVector3(direction.x, direction.y, direction.z);
    _world->rayTest(bvpos, bvdir, callback);
}

void PhysicsWorld::ConvexSweepTest(const btConvexShape* shape,
    const btTransform& from,
    const btTransform& to,
    btCollisionWorld::ConvexResultCallback& callback,
    float allowedCcdPenetration)
{
    return _world->convexSweepTest(shape, from, to, callback, allowedCcdPenetration);
}