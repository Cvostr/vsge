#include "ApiBindings.hpp"
#include <Scene/SceneLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Resources/ResourceCache.hpp>
#include <Resources/ResourceTypes/SceneResource.hpp>

using namespace VSGE;

static void AddScene() {
	SceneLayer::Get()->AddScene();
}

static Scene* GetMainScene() {
	return SceneLayer::Get()->GetMainScene();
}

static void LoadScene(MonoString* scene_resource_name, uint32 slot) {
	std::string name = std::string(mono_string_to_utf8(scene_resource_name));
	SceneResource* resource = ResourceCache::Get()->GetResource<SceneResource>(name);
	Scene* scn = SceneLayer::Get()->GetScenes()[slot];
	scn->Stop();
	scn->NewScene();
	if (resource) {
		resource->LoadAsync(slot);
	}
	scn->RunOnLoad();
}

static MonoArray* GetScenes() {
	std::vector<Scene*>& scenes = SceneLayer::Get()->GetScenes();
	MonoArray* result = mono_array_new(MonoScriptingLayer::Get()->GetDomain(),
		mono_get_uint64_class(), scenes.size());

	for (uint32 c_i = 0; c_i < scenes.size(); c_i++) {
		uint64 pointer = (uint64)scenes[c_i];
		mono_array_set(result, uint64, c_i, pointer);
	}

	return result;
}

void VSGE::BindScenesApi() {
	MonoScriptingLayer::AddInternalCall("Scenes::AddScene()", AddScene);
	MonoScriptingLayer::AddInternalCall("Scenes::i_GetMainScene()", GetMainScene);
	MonoScriptingLayer::AddInternalCall("Scenes::LoadScene(string,uint)", LoadScene);
	MonoScriptingLayer::AddInternalCall("Scenes::i_GetScenes()", GetScenes);
}