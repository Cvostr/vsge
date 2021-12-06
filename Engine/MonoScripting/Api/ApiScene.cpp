#include "ApiBindings.hpp"
#include <Scene/Scene.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

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
	MonoScriptingLayer::AddInternalCall("Scene::i_GetRootNode(ulong)", GetRootEntity);
	MonoScriptingLayer::AddInternalCall("Scene::i_GetEntity(ulong,string)", GetEntityByName);
	MonoScriptingLayer::AddInternalCall("Scene::i_AddNewEntity(ulong,string)", AddNewEntity);
}