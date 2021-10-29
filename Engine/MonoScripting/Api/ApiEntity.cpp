#include "ApiBindings.hpp"
#include "ApiEntity.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/AudioSourceComponent.hpp>
#include <Scene/EntityComponents/RigidBodyComponent.hpp>
#include <Scene/EntityComponents/ColliderComponent.hpp>
#include <Scene/EntityComponents/AnimatorComponent.hpp>
#include <Scene/EntityComponents/ParticleEmitterComponent.hpp>
#include <Scene/EntityComponents/CharacterControllerComponent.hpp>

using namespace VSGE;

static void* GetParent(Entity* ptr) {
	return ptr->GetParent();
}

static void AddChild(Entity* ptr, Entity* child) {
	ptr->AddChild(child);
}

static void RemoveChild(Entity* ptr, Entity* child) {
	ptr->RemoveChild(child);
}

static MonoArray* GetChildren(void* ptr) {
	Entity* ent = ((Entity*)ptr);
	MonoArray* result = mono_array_new(MonoScriptingLayer::Get()->GetDomain(), mono_get_uint64_class(), ent->GetChildrenCount());
	for (int32 c_i = 0; c_i < (int32)ent->GetChildrenCount(); c_i ++) {
		uint64 child = (uint64)ent->GetChildren()[c_i];
		mono_array_set(result, uint64, c_i, child);
	}
	return result;
}

static AABB GetAABB(Entity* ptr) {
	return ptr->GetAABB();
}

static void* GetScene(Entity* ptr) {
	return ptr->GetScene();
}

static void SetActive(Entity* ptr, bool active) {
	ptr->SetActive(active);
}

static bool IsActive(Entity* ptr) {
	return ptr->IsActive();
}

static void SetName(Entity* ptr, MonoString* name) {
	ptr->SetName(std::string(mono_string_to_utf8(name)));
}

static MonoString* GetName(Entity* ptr) {
	MonoScriptingLayer* layer = MonoScriptingLayer::Get();
	return mono_string_new(layer->GetDomain(), ptr->GetName().c_str());
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

static void* WComponent(void* ptr, int type, int action) {
	Entity* ent = (Entity*)ptr;
	switch (type) {
	case ENTITY_COMPONENT_MATERIAL:
		return _WComponent<MaterialComponent>(ent, action);
		break;
	case ENTITY_COMPONENT_MESH:
		return _WComponent<MeshComponent>(ent, action);
		break;
	case ENTITY_COMPONENT_ANIMATOR:
		return _WComponent<AnimatorComponent>(ent, action);
		break;
	case ENTITY_COMPONENT_LIGHTSOURCE:
		return _WComponent<LightsourceComponent>(ent, action);
		break;
	case ENTITY_COMPONENT_AUDIO_SOURCE:
		return _WComponent<AudioSourceComponent>(ent, action);
		break;
	case ENTITY_COMPONENT_RIGIDBODY:
		return _WComponent<RigidBodyComponent>(ent, action);
		break;
	case ENTITY_COMPONENT_COLLIDER:
		return _WComponent<ColliderComponent>(ent, action);
		break;
	case ENTITY_COMPONENT_CAMERA:
		return _WComponent<Camera>(ent, action);
		break;
	case ENTITY_COMPONENT_PARTICLE_EMITTER:
		return _WComponent<ParticleEmitterComponent>(ent, action);
		break;
	case ENTITY_COMPONENT_CHARACTER_CONTROLLER:
		return _WComponent<ParticleEmitterComponent>(ent, action);
		break;
	}
	return nullptr;
}

MonoObject* GetScript(Entity* ptr, MonoString* script_class_name) {
	std::string name = std::string(mono_string_to_utf8(script_class_name));
	for (uint32 i = 0; i < ptr->GetScriptsCount(); i++) {
		EntityScriptComponent* script = ptr->GetScripts()[i];
		if (script->GetClassName() == name) {
			return script->GetInstance()->GetObject();
		}
	}
	return nullptr;
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

	mono_add_internal_call("Entity::WComponent(ulong,int,int)", WComponent);
	mono_add_internal_call("Entity::i_GetScript(ulong,string)", GetScript);
}