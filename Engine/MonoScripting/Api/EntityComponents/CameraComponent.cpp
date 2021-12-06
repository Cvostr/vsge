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

static float GetAspectRatio(Camera* cam) {
	return cam->GetAspectRatio();
}
static void SetAspectRatio(Camera* cam, float aspect) {
	cam->SetAspectRatio(aspect);
}

static ViewMask GetViewMask(Camera* cam) {
	return cam->GetViewMask();
}
static void SetViewMask(Camera* cam, ViewMask viewmask) {
	cam->SetViewMask(viewmask);
}

void VSGE::BindCamera() {
	MonoScriptingLayer::AddInternalCall("CameraComponent::i_GetNearPlane(ulong)", GetNearPlane);
	MonoScriptingLayer::AddInternalCall("CameraComponent::i_SetNearPlane(ulong,float)", SetNearPlane);

	MonoScriptingLayer::AddInternalCall("CameraComponent::i_GetFarPlane(ulong)", GetFarPlane);
	MonoScriptingLayer::AddInternalCall("CameraComponent::i_SetFarPlane(ulong,float)", SetFarPlane);

	MonoScriptingLayer::AddInternalCall("CameraComponent::i_GetFov(ulong)", GetFov);
	MonoScriptingLayer::AddInternalCall("CameraComponent::i_SetFarPlane(ulong,float)", SetFov);

	MonoScriptingLayer::AddInternalCall("CameraComponent::i_GetAspectRatio(ulong)", GetAspectRatio);
	MonoScriptingLayer::AddInternalCall("CameraComponent::i_SetAspectRatio(ulong,float)", SetAspectRatio);

	MonoScriptingLayer::AddInternalCall("CameraComponent::i_GetViewMask(ulong)", GetViewMask);
	MonoScriptingLayer::AddInternalCall("CameraComponent::i_SetViewMask(ulong,ulong)", SetViewMask);
}