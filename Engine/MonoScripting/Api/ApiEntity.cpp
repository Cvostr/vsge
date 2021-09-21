#include "ApiBindings.hpp"
#include <Scene/Entity.hpp>

using namespace VSGE;

static void SetActive(void* ptr, bool active) {
	((Entity*)ptr)->SetActive(active);
}

static bool IsActive(void* ptr) {
	return ((Entity*)ptr)->IsActive();
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

void VSGE::BindEntityApi() {
	mono_add_internal_call("Entity::i_SetActive(ulong,bool)", SetActive);
	mono_add_internal_call("Entity::i_IsActive(ulong)", IsActive);

	mono_add_internal_call("Entity::i_SetPosition(ulong,Vec3)", SetPosition);
	mono_add_internal_call("Entity::i_GetPosition(ulong)", GetPosition);
	mono_add_internal_call("Entity::i_SetScale(ulong,Vec3)", SetScale);
	mono_add_internal_call("Entity::i_GetScale(ulong)", GetScale);
}