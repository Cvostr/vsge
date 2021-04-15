#include "Scene.hpp"
#include "Core/Random.hpp"

using namespace VSGE;

void Scene::NewScene() {
	mRootEntity = new Entity;
	mRootEntity->SetName("Root");
	mRootEntity->SetScene(this);
}

Entity* Scene::AddNewEntity() {
	int addition = 0;
	std::string name_to_test = "Entity";
	while (GetEntityWithName(name_to_test)) {
		addition++;
		name_to_test = "Entity (" + std::to_string(addition) + ")";
	}

	return AddNewEntity(name_to_test);
}

Entity* Scene::AddNewEntity(std::string name) {
	auto entity = new Entity;
	entity->SetName(name);
	entity->SetScene(this);
	if (mRootEntity)
		mRootEntity->AddChild(entity);
	return entity;
}

Entity* Scene::GetEntityWithName(std::string name) {
	return mRootEntity->GetEntityWithName(name);
}

Entity* Scene::GetEntityWithGuid(const Guid& id) {
	return mRootEntity->GetEntityWithGuid(id);
}