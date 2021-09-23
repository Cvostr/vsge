#include "ApiBindings.hpp"
#include <Scene/Scene.hpp>

using namespace VSGE;

static void* GetRootEntity(void* scene) {
	return ((Scene*)scene)->GetRootEntity();
}

static void* GetEntityByName(void* scene, MonoString* name) {
	return ((Scene*)scene)->GetEntityWithName(std::string(mono_string_to_utf8(name)));
}

static void* AddNewEntity(void* scene, MonoString* name) {
	return ((Scene*)scene)->AddNewEntity(std::string(mono_string_to_utf8(name)));
}

void VSGE::BindSceneApi() {
	mono_add_internal_call("Scene::i_GetRootNode(ulong)", GetRootEntity);
	mono_add_internal_call("Scene::i_GetEntity(ulong,string)", GetEntityByName);
	mono_add_internal_call("Scene::i_AddNewEntity(ulong,string)", AddNewEntity);
}