#include "Scene.hpp"
#include "Core/Random.hpp"

using namespace VSGE;

void Scene::NewScene() {
	if (mRootEntity) {
		mRootEntity->Destroy();
	}
	mRootEntity = new Entity;
	mRootEntity->SetScene(this);
	mRootEntity->SetName("Root");
	mRootEntity->SetGuid(Guid(0, 0, 0, 0));
}

Entity* Scene::AddNewEntity(const std::string& name) {
	auto entity = new Entity;
	entity->SetScene(this);
	entity->SetName(name);

	if (mRootEntity)
		mRootEntity->AddChild(entity);
	return entity;
}

Entity* Scene::GetEntityWithName(const std::string& name) {
	return mRootEntity->GetEntityWithName(name);
}

Entity* Scene::GetEntityWithGuid(const Guid& id) {
	return mRootEntity->GetEntityWithGuid(id);
}