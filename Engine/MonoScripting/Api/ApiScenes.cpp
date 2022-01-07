#include "ApiBindings.hpp"
#include <Scene/SceneLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>]
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

void VSGE::BindScenesApi() {
	MonoScriptingLayer::AddInternalCall("Scenes::AddScene()", AddScene);
	MonoScriptingLayer::AddInternalCall("Scenes::i_GetMainScene()", GetMainScene);
	MonoScriptingLayer::AddInternalCall("Scenes::LoadScene(string,uint)", LoadScene);
}