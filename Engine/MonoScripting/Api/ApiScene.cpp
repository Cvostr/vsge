#include "ApiBindings.hpp"
#include <Scene/Scene.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void* GetRootEntity(Scene* scene) {
	return scene->GetRootEntity();
}

static void* GetEntityByName(Scene* scene, MonoString* name) 
{
	char* pName = mono_string_to_utf8(name);
	std::string sName(pName);
	mono_free(pName);

	return scene->GetEntityWithName(sName);
}

static void* AddNewEntity(Scene* scene, MonoString* name) 
{
	char* pName = mono_string_to_utf8(name);
	std::string sName(pName);
	mono_free(pName);

	return scene->AddNewEntity(sName);
}

static void Run(Scene* scene) {
	scene->Run();
}

static void Pause(Scene* scene) {
	scene->Pause();
}

static void Stop(Scene* scene) {
	scene->Stop();
}

static bool IsRunning(Scene* scene) {
	return scene->IsSceneRunning();
}

static bool IsPaused(Scene* scene) {
	return scene->IsScenePaused();
}

static MonoArray* GetAllEntitiesByName(Scene* scene, MonoString* name) 
{
	char* pName = mono_string_to_utf8(name);
	std::string sName = std::string(mono_string_to_utf8(name));
	mono_free(pName);

	std::vector<Entity*> entities = scene->GetAllEntitiesWithName(sName);
	MonoArray* result = mono_array_new(MonoScriptingLayer::Get()->GetDomain(), 
		mono_get_uint64_class(), entities.size());
	
	for (uint32 c_i = 0; c_i < entities.size(); c_i++) {
		uint64 pointer = (uint64)entities[c_i];
		mono_array_set(result, uint64, c_i, pointer);
	}

	return result;
}

void VSGE::BindSceneApi() {
	MonoScriptingLayer::AddInternalCall("Scene::i_GetRootNode(ulong)", GetRootEntity);
	MonoScriptingLayer::AddInternalCall("Scene::i_GetEntity(ulong,string)", GetEntityByName);
	MonoScriptingLayer::AddInternalCall("Scene::i_AddNewEntity(ulong,string)", AddNewEntity);
	MonoScriptingLayer::AddInternalCall("Scene::i_GetAllEntitiesByName(ulong,string)", GetAllEntitiesByName);

	MonoScriptingLayer::AddInternalCall("Scene::i_Run(ulong)", Run);
	MonoScriptingLayer::AddInternalCall("Scene::i_Pause(ulong)", Pause);
	MonoScriptingLayer::AddInternalCall("Scene::i_Stop(ulong)", Stop);
	MonoScriptingLayer::AddInternalCall("Scene::i_IsRunning(ulong)", IsRunning);
	MonoScriptingLayer::AddInternalCall("Scene::i_IsPaused(ulong)", IsPaused);
}