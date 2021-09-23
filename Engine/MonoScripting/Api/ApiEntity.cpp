#include "ApiBindings.hpp"
#include <Scene/Entity.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void* GetParent(void* ptr) {
	return ((Entity*)ptr)->GetParent();
}

static void AddChild(void* ptr, void* child) {
	((Entity*)ptr)->AddChild((Entity*)child);
}

static void RemoveChild(void* ptr, void* child) {
	((Entity*)ptr)->RemoveChild((Entity*)child);
}

static MonoArray* GetChildren(void* ptr) {
	//MonoArray* result = mono_array_new()
	return nullptr;
}

static AABB GetAABB(void* ptr) {
	return ((Entity*)ptr)->GetAABB();
}

static void* GetScene(void* ptr) {
	return ((Entity*)ptr)->GetScene();
}

static void SetActive(void* ptr, bool active) {
	((Entity*)ptr)->SetActive(active);
}

static bool IsActive(void* ptr) {
	return ((Entity*)ptr)->IsActive();
}

static void SetName(void* ptr, MonoString* name) {
	((Entity*)ptr)->SetName(std::string(mono_string_to_utf8(name)));
}

static MonoString* GetName(void* ptr) {
	MonoScriptingLayer* layer = MonoScriptingLayer::Get();
	return mono_string_new(layer->GetDomain(), ((Entity*)ptr)->GetName().c_str());
}

static void SetPosition(void* ptr, Vec3 position) {
	((Entity*)ptr)->SetPosition(position);
}

static Vec3 GetPosition(void* ptr) {
	return ((Entity*)ptr)->GetPosition();
}

static void SetScale(void* ptr, Vec3 scale) {
	((Entity*)ptr)->SetScale(scale);
}

static Vec3 GetScale(void* ptr) {
	return ((Entity*)ptr)->GetScale();
}

static void SetRotation(void* ptr, Quat rotation) {
	((Entity*)ptr)->SetRotation(rotation);
}

static Quat GetRotation(void* ptr) {
	return ((Entity*)ptr)->GetRotation();
}


void VSGE::BindEntityApi() {
	mono_add_internal_call("Entity::i_GetParent(ulong)", GetParent);
	mono_add_internal_call("Entity::i_AddChild(ulong,ulong)", AddChild);
	mono_add_internal_call("Entity::i_RemoveChild(ulong,ulong)", RemoveChild);
	mono_add_internal_call("Entity::i_GetChildren(ulong)", GetChildren);
	mono_add_internal_call("Entity::i_GetScene(ulong)", GetScene);
	mono_add_internal_call("Entity::i_GetAABB(ulong)", GetAABB);

	mono_add_internal_call("Entity::i_SetActive(ulong,bool)", SetActive);
	mono_add_internal_call("Entity::i_IsActive(ulong)", IsActive);
	mono_add_internal_call("Entity::i_SetName(ulong,string)", SetName);
	mono_add_internal_call("Entity::i_GetName(ulong)", GetName);

	mono_add_internal_call("Entity::i_SetPosition(ulong,Vec3)", SetPosition);
	mono_add_internal_call("Entity::i_GetPosition(ulong)", GetPosition);
	mono_add_internal_call("Entity::i_SetScale(ulong,Vec3)", SetScale);
	mono_add_internal_call("Entity::i_GetScale(ulong)", GetScale);
	mono_add_internal_call("Entity::i_SetRotation(ulong,Quat)", SetRotation);
	mono_add_internal_call("Entity::i_GetRotation(ulong)", GetRotation);
}