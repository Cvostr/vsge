#include "SceneLayer.hpp"
#include "SceneSerialization.hpp"

using namespace VSGE;

SceneLayer* SceneLayer::_this = nullptr;

SceneLayer::SceneLayer() {
	AddScene();

	_scene_backup_data = nullptr;
	_scene_backup_size = 0;

	_this = this;
}

void SceneLayer::AddScene() {
	Scene* scene = new Scene;
	scene->NewScene();
	_scenes.push_back(scene);
}

std::vector<Scene*>& SceneLayer::GetScenes() {
	return _scenes;
}

Scene* SceneLayer::GetMainScene() {
	return _scenes[0];
}

void SceneLayer::OnAttach() {

}
void SceneLayer::OnDetach() {

}

void SceneLayer::OnUpdate() {
	_scenes[0]->Update();
}

void SceneLayer::StartScene(uint32 index) {
	_scenes[index]->Run();
}

void SceneLayer::StopScene(uint32 index) {
	_scenes[index]->Stop();
}

void SceneLayer::PauseScene(uint32 index) {
	_scenes[index]->Pause();
}

bool SceneLayer::IsSceneRunning(uint32 index) {
	return _scenes[index]->IsSceneRunning();
}

bool SceneLayer::IsScenePaused(uint32 index) {
	return _scenes[index]->IsScenePaused();
}

void SceneLayer::BackupScene() {
	BackupScene(&_scene_backup_data, _scene_backup_size);
}
void SceneLayer::BackupScene(byte** data, uint32& size) {
	VSGE::SceneSerializer sc;
	sc.SetScene(_scenes[0]);
	sc.SerializeBinary(data, size);
}
void SceneLayer::RestoreScene() {
	RestoreScene(_scene_backup_data, _scene_backup_size);
	SAFE_RELEASE_ARR(_scene_backup_data)
}
void SceneLayer::RestoreScene(byte* data, uint32 size) {
	VSGE::SceneSerializer sc;
	sc.SetScene(_scenes[0]);
	sc.DeserializeBinary(data, size);
}