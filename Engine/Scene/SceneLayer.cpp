#include "SceneLayer.hpp"
#include "SceneSerialization.hpp"

using namespace VSGE;

SceneLayer* SceneLayer::_this = nullptr;

SceneLayer::SceneLayer() {
	_scene = nullptr;
	_scene_running = false;
	_scene_paused = false;
	_scene_backup_data = nullptr;
	_scene_backup_size = 0;

	_this = this;
}
void SceneLayer::SetWorkingScene(Scene* scene) {
	_scene = scene;
}
Scene* SceneLayer::GetWorkingScene(){
	return _scene;
}

void SceneLayer::StartScene() {
	if (_scene_running)
		return;

	_scene_running = true;
	_scene_paused = false;

	if (_scene) {
		CallOnStart(_scene->GetRootEntity());
	}
}

void SceneLayer::StopScene() {
	if (!_scene_running)
		return;

	_scene_running = false;
	_scene_paused = false;

	if (_scene) {
		CallOnStop(_scene->GetRootEntity());
	}
}

void SceneLayer::PauseScene() {
	if (!_scene_running)
		return;

	_scene_running = false;
	_scene_paused = true;
}

bool SceneLayer::IsSceneRunning() {
	return _scene_running;
}

bool SceneLayer::IsScenePaused() {
	return _scene_paused;
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

void SceneLayer::BackupScene() {
	BackupScene(&_scene_backup_data, _scene_backup_size);
}
void SceneLayer::BackupScene(byte** data, uint32& size) {
	VSGE::SceneSerializer sc;
	sc.SetScene(_scene);
	sc.SerializeBinary(data, size);
}
void SceneLayer::RestoreScene() {
	RestoreScene(_scene_backup_data, _scene_backup_size);
	SAFE_RELEASE_ARR(_scene_backup_data)
}
void SceneLayer::RestoreScene(byte* data, uint32 size) {
	VSGE::SceneSerializer sc;
	sc.SetScene(_scene);
	sc.DeserializeBinary(data, size);
}