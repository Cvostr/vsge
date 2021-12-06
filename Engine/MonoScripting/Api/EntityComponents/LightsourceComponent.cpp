#include "../ApiBindings.hpp"
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Math/Color.hpp>
#include <Math/Vec3.hpp>

using namespace VSGE;

Color GetColor(void* ptr) {
	return ((LightsourceComponent*)ptr)->GetColor();
}
void SetColor(void* ptr, Color color) {
	((LightsourceComponent*)ptr)->GetColor() = color;
}

float GetIntensity(LightsourceComponent* ptr) {
	return (ptr)->GetIntensity();
}
void SetIntensity(void* ptr, float intensity) {
	((LightsourceComponent*)ptr)->GetIntensity() = intensity;
}

float GetRange(void* ptr) {
	return ((LightsourceComponent*)ptr)->GetRange();
}
void SetRange(void* ptr, float range) {
	((LightsourceComponent*)ptr)->GetRange() = range;
}

float GetSpotAngle(void* ptr) {
	return ((LightsourceComponent*)ptr)->GetSpotAngle();
}
void SetSpotAngle(void* ptr, float spot_angle) {
	((LightsourceComponent*)ptr)->GetSpotAngle() = spot_angle;
}

Vec3 GetDirection(LightsourceComponent* ptr) {
	return ptr->GetDirection();
}

float IsCastShadows(LightsourceComponent* ptr) {
	return ptr->GetCastShadows();
}
void SetCastShadows(LightsourceComponent* ptr, bool cast) {
	ptr->SetCastShadows(cast);
}

void VSGE::BindLightsource() {
	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_GetIntensity(ulong)", GetIntensity);
	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_SetIntensity(ulong,float)", SetIntensity);

	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_GetRange(ulong)", GetRange);
	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_SetRange(ulong,float)", SetRange);

	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_GetSpotAngle(ulong)", GetSpotAngle);
	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_SetSpotAngle(ulong,float)", SetSpotAngle);

	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_SetColor(ulong,Color)", SetColor);
	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_GetColor(ulong)", GetColor);

	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_GetDirection(ulong)", GetDirection);

	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_IsCastShadows(ulong)", IsCastShadows);
	MonoScriptingLayer::AddInternalCall("LightSourceComponent::i_SetCastShadows(ulong,bool)", SetCastShadows);
}