#include "../ApiBindings.hpp"
#include <Scene/Camera.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static float GetNearPlane(Camera* cam) {
	return cam->GetNearPlane();
}
static void SetNearPlane(Camera* cam, float near) {
	cam->SetNearPlane(near);
}

static float GetFarPlane(Camera* cam) {
	return cam->GetFarPlane();
}
static void SetFarPlane(Camera* cam, float far) {
	cam->SetFarPlane(far);
}

static float GetFov(Camera* cam) {
	return cam->GetFOV();
}
static void SetFov(Camera* cam, float fov) {
	cam->SetFOV(fov);
}

void VSGE::BindCamera() {
	mono_add_internal_call("CameraComponent::i_GetNearPlane(ulong)", GetNearPlane);
	mono_add_internal_call("CameraComponent::i_SetNearPlane(ulong,float)", SetNearPlane);

	mono_add_internal_call("CameraComponent::i_GetFarPlane(ulong)", GetFarPlane);
	mono_add_internal_call("CameraComponent::i_SetFarPlane(ulong,float)", SetFarPlane);

	mono_add_internal_call("CameraComponent::i_GetFov(ulong)", GetFov);
	mono_add_internal_call("CameraComponent::i_SetFarPlane(ulong,float)", SetFov);
}