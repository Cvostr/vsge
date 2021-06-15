#include "SceneLayer.hpp"

using namespace VSGE;

SceneLayer::SceneLayer() {
	_scene = nullptr;
	_scene_running = false;
}
void SceneLayer::SetWorkingScene(Scene* scene) {
	_scene = scene;
}
Scene* SceneLayer::GetWorkingScene(){
	return _scene;
}

void SceneLayer::StartScene() {
	_scene_running = true;

	if (_scene) {
		CallOnStart(_scene->GetRootEntity());
	}
}

void SceneLayer::StopScene() {
	_scene_running = false;

	if (_scene) {
		CallOnStop(_scene->GetRootEntity());
	}
}

bool SceneLayer::IsSceneRunning() {
	return _scene_running;
}

void SceneLayer::OnAttach() {

}
void SceneLayer::OnDetach() {

}

void SceneLayer::CallOnStart(Entity* entity) {
	entity->CallOnStart();

	uint32 children_count = entity->GetChildrenCount();
	for (uint32 child_i = 0; child_i < children_count; child_i++) {
		CallOnStart(entity->GetChildren()[child_i]);
	}
}

void SceneLayer::CallOnUpdate(Entity* entity) {
	entity->CallOnUpdate();

	uint32 children_count = entity->GetChildrenCount();
	for (uint32 child_i = 0; child_i < children_count; child_i++) {
		CallOnUpdate(entity->GetChildren()[child_i]);
	}
}

void SceneLayer::CallOnStop(Entity* entity) {
	entity->CallOnStop();

	uint32 children_count = entity->GetChildrenCount();
	for (uint32 child_i = 0; child_i < children_count; child_i++) {
		CallOnStop(entity->GetChildren()[child_i]);
	}
}

void SceneLayer::OnUpdate() {
	if (_scene_running && _scene) {
		CallOnUpdate(_scene->GetRootEntity());
	}
}