#include "SceneLayer.hpp"
#include "SceneSerialization.hpp"
#include <Engine/Application.hpp>

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
	for(auto& scene : _scenes)
		scene->Update();
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

bool SceneLayer::IsSceneLoading(uint32 index) {
	return _scenes[index]->isLoading();
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

void SceneLayer::LoadSceneAsync(SceneResource* resource, uint32 index, bool runOnLoad)
{
	if (_scenes.size() <= index)
		return;

	Application::Get()->QueueTask([&, resource, index, runOnLoad] {

		SceneLoadBeginEvent* load_event = new SceneLoadBeginEvent;
		Application::Get()->QueueEvent(load_event);

		resource->ReadData();

		Scene* newScene = new Scene();
		newScene->NewScene();
		newScene->setLoading(true);

		VSGE::SceneSerializer ss;
		ss.SetScene(newScene);
		ss.SetPermanentScene(_scenes[index]);
		bool rc = ss.DeserializeBinary(resource->GetLoadedData(), resource->GetDataDescription().size);

		// Clean up
		resource->Release();

		newScene->setLoading(false);
		SceneLoadedEvent* loaded_event = new SceneLoadedEvent(index, newScene, runOnLoad);
		Application::Get()->QueueEvent(loaded_event);
	});
}

void SceneLayer::OnEvent(const VSGE::IEvent& event) {
	DispatchEvent<VSGE::SceneLoadedEvent>(event, EVENT_FUNC(SceneLayer::OnSceneLoadedEvent));
}

void SceneLayer::OnSceneLoadedEvent(const VSGE::SceneLoadedEvent& event) 
{
	Scene& scn = *_scenes[event.GetSlot()];
	scn = std::move(*event.GetScene());
	delete event.GetScene();

	if (event.IsRunOnLoad())
		scn.Run();
}