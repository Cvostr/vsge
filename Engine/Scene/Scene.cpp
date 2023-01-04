#include "Scene.hpp"
#include "Core/Random.hpp"
#include <Core/ByteSolver.hpp>
#include "SceneSerialization.hpp"

using namespace VSGE;

Scene::Scene() :
	_rootEntity(nullptr) ,
	_running(false),
	_paused(false),
	_delayed_run(false),
	_physics_world(nullptr)
{}

Scene::~Scene() {
	Destroy();
}

void Scene::Destroy() {
	if (_rootEntity) {
		_rootEntity->Destroy();
	}
	if (_physics_world)
		SAFE_RELEASE(_physics_world)
}

void Scene::NewScene() {
	Destroy();
	_rootEntity = new Entity;
	_rootEntity->SetScene(this);
	_rootEntity->SetName("Root");
	_rootEntity->SetGuid(Guid(0, 0, 0, 0));

	_physics_world = new PhysicsWorld;
	_physics_world->Create();
}

Entity* Scene::GetRootEntity() {
	return _rootEntity; 
}

Entity* Scene::AddNewEntity(const std::string& name) {
	auto entity = new Entity;
	entity->SetScene(this);
	entity->SetName(name);

	if (_rootEntity)
		_rootEntity->AddChild(entity);
	return entity;
}

Entity* Scene::GetEntityWithName(const std::string& name) const {
	return _rootEntity->GetEntityWithName(name);
}

std::vector<Entity*> Scene::GetAllEntitiesWithName(const std::string& name) const {
	std::vector<Entity*> result;
	_rootEntity->GetEntitiesWithName(name, result);
	return result;
}

Entity* Scene::GetEntityWithGuid(const Guid& id) {
	return _rootEntity->GetEntityWithGuid(id);
}

uint32 Scene::GetEntitiesCount() {
	return _rootEntity->GetTotalChildrenCount();
}

void Scene::AddFromPrefab(byte* data, uint32 size) {
	ByteSolver solver(data, size);

	uint32 entities_count = solver.GetValue<uint32>();
	SceneSerializer serializer;
	serializer.SetScene(this);

	std::vector<tGuidPair> aliases;
	Guid zero_id = Guid(0, 0, 0, 0);
	aliases.push_back(std::make_pair(zero_id, zero_id));

	for (uint32 entity_i = 0; entity_i < entities_count; entity_i++) {
		Entity* ent = new Entity;
		Guid parent_id;
		serializer.DeserializeEntityBinary(ent, solver, &parent_id);
		Guid old_id = ent->GetGuid();
		ent->SetGuid(Guid());
		Guid new_id = ent->GetGuid();
		aliases.push_back(std::make_pair(old_id, new_id));

		Guid new_parent_id;

		for (auto alias : aliases) {
			if (alias.first == parent_id) {
				new_parent_id = alias.second;
			}
		}
		Entity* parent = GetEntityWithGuid(new_parent_id);
		if (!parent)
			parent = _rootEntity;
		parent->AddChild(ent);
	}
}

AudioListenerComponent* Scene::GetAudioListener(Entity* ent) {
	if (ent == nullptr)
		ent = _rootEntity;

	AudioListenerComponent* listener = ent->GetComponent<AudioListenerComponent>();

	if (listener)
		return listener;
	else
		for (uint32 child_i = 0; child_i < ent->GetChildrenCount(); child_i++) {
			listener = GetAudioListener(ent->GetChildren()[child_i]);

			if (listener)
				return listener;
		}


	return listener;
}

AABB Scene::GetSceneBoundingBox() {
	if(_rootEntity)
		return _rootEntity->GetAABB();
	return AABB();
}

void Scene::CallOnStart(Entity* entity) {
	entity->CallOnStart();

	uint32 children_count = entity->GetChildrenCount();
	for (uint32 child_i = 0; child_i < children_count; child_i++) {
		CallOnStart(entity->GetChildren()[child_i]);
	}
}

void Scene::CallOnUpdate(Entity* entity) {
	entity->CallOnUpdate();

	uint32 children_count = entity->GetChildrenCount();
	for (uint32 child_i = 0; child_i < children_count; child_i++) {
		CallOnUpdate(entity->GetChildren()[child_i]);
	}
}

void Scene::CallOnStop(Entity* entity) {
	entity->CallOnStop();

	uint32 children_count = entity->GetChildrenCount();
	for (uint32 child_i = 0; child_i < children_count; child_i++) {
		CallOnStop(entity->GetChildren()[child_i]);
	}
}

bool Scene::IsSceneRunning() {
	return _running;
}

bool Scene::IsScenePaused() {
	return _paused;
}

bool Scene::setLoading(bool loading) {
	this->_loading = loading;
}

bool Scene::isLoading() {
	return _loading;
}

void Scene::Run() {
	if (_running)
		return;

	bool was_paused = _paused;

	_running = true;
	_paused = false;
	_delayed_run = false;

	_physics_world->SetActive(true);

	if (!was_paused) {
		CallOnStart(GetRootEntity());
	}
}

void Scene::RunOnLoad() {
	_delayed_run = true;
}

bool Scene::IsDelayedRun() {
	return _delayed_run;
}

void Scene::Pause() {
	if (!_running)
		return;

	_running = false;
	_paused = true;

	_physics_world->SetActive(false);
}

void Scene::Stop() {
	if (!_running)
		return;

	_running = false;
	_paused = false;

	_physics_world->SetActive(false);

	CallOnStop(GetRootEntity());
}

void Scene::Update() {
	if (_running) {
		CallOnUpdate(GetRootEntity());
	}
}

void Scene::UpdateSceneTree(const Vec3& size) {
	AABB scene_bb = GetSceneBoundingBox();

	Vec3 delta = scene_bb.GetMax() - scene_bb.GetMin();
	//amount of bounding boxes in all three dimensions
	Vec3i counts = Vec3i(
		(int)ceil(delta.x / size.x),
		(int)ceil(delta.y / size.y),
		(int)ceil(delta.z / size.z)
	);
	//fill scene tree with bounding boxes
	for (uint32 x = 0; x < counts.x; x++) {
		for (uint32 y = 0; y < counts.y; y++) {
			for (uint32 z = 0; z < counts.z; z++) {
				Vec3 min = scene_bb.GetMin() + Vec3(x, y, z) * size;
				Vec3 max = min + size;
				AABB bb = AABB(min, max);
				//_scene_tree.insert(std::make_pair(bb, std::vector<Entity*>()));
			}
		}
	}

	for (uint32 i = 0; i < _scene_tree.size(); i++) {
		//GetEntitiesIntersects(_scene_tree.at());
	}
}

void Scene::GetEntitiesIntersects(const AABB& bb, std::vector<Entity*>& array, Entity* first) {
	if (first == nullptr)
		first = _rootEntity;
	else
		if (bb.IsIntersects(first->GetAABB()))
			array.push_back(first);

	for (uint32 c_i = 0; c_i < first->GetChildrenCount(); c_i++) {
		GetEntitiesIntersects(bb, array, first->GetChildren()[c_i]);
	}
}

SceneEnvironmentSettings& Scene::GetEnvironmentSettings(){
	return _environment_settings;
}

PhysicsWorld* Scene::GetPhysicalWorld() {
	return _physics_world;
}