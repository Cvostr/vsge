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
}

Entity* Scene::AddNewEntity(const std::string& name) {
	int addition = 0;
	std::string name_to_test = name;
	while (GetEntityWithName(name_to_test)) {
		addition++;
		name_to_test = name + " (" + std::to_string(addition) + ")";
	}

	auto entity = new Entity;
	entity->SetScene(this);
	entity->SetName(name_to_test);

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

void Scene::Serialize(Serializer& s) {
	s.BeginHeader("scene");

	mRootEntity->Serialize(s);

	s.EndHeader();
}

void Scene::DeSerialize(Deserializer& ds) {
	std::string header = ds.GetNextHeaderName();

	if (header != "scene")
		return;

	while (!ds.End()) {
		std::string header = ds.GetNextHeaderName();

		if (header == "Entity") {
			Entity* ent = new Entity;
			ent->SetScene(this);
			ent->DeSerialize(ds);
			
		}
	}
}