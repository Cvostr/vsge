#include "Scene.hpp"
#include "Core/Random.hpp"
#include <Core/ByteSolver.hpp>
#include "SceneSerialization.hpp"

using namespace VSGE;

Scene::Scene() :
	_rootEntity(nullptr) 
{}

void Scene::NewScene() {
	if (_rootEntity) {
		_rootEntity->Destroy();
	}
	_rootEntity = new Entity;
	_rootEntity->SetScene(this);
	_rootEntity->SetName("Root");
	_rootEntity->SetGuid(Guid(0, 0, 0, 0));
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

Entity* Scene::GetEntityWithName(const std::string& name) {
	return _rootEntity->GetEntityWithName(name);
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

		GetEntityWithGuid(new_parent_id)->AddChild(ent);
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