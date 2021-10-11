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
	mono_add_internal_call("LightSourceComponent::i_GetIntensity(ulong)", GetIntensity);
	mono_add_internal_call("LightSourceComponent::i_SetIntensity(ulong,float)", SetIntensity);

	mono_add_internal_call("LightSourceComponent::i_GetRange(ulong)", GetRange);
	mono_add_internal_call("LightSourceComponent::i_SetRange(ulong,float)", SetRange);

	mono_add_internal_call("LightSourceComponent::i_GetSpotAngle(ulong)", GetSpotAngle);
	mono_add_internal_call("LightSourceComponent::i_SetSpotAngle(ulong,float)", SetSpotAngle);

	mono_add_internal_call("LightSourceComponent::i_SetColor(ulong,Color)", SetColor);
	mono_add_internal_call("LightSourceComponent::i_GetColor(ulong)", GetColor);

	mono_add_internal_call("LightSourceComponent::i_GetDirection(ulong)", GetDirection);

	mono_add_internal_call("LightSourceComponent::i_IsCastShadows(ulong)", IsCastShadows);
	mono_add_internal_call("LightSourceComponent::i_SetCastShadows(ulong,bool)", SetCastShadows);
}