#include "SceneEnvironment.hpp"
#include "math.h"

using namespace VSGE;

SceneEnvironmentSettings::SceneEnvironmentSettings(){
	_skybox_material.SetResourceType(RESOURCE_TYPE_MATERIAL);
	_shadow_distance = 200;
	_shadow_cascades_count = 4;
	UpdateShadows();
}

void SceneEnvironmentSettings::UpdateShadows(){
	_cascade_depths.resize(_shadow_cascades_count);
	_cascade_dists.resize(_shadow_cascades_count);
	
	float nearClip = 0.1f;
	float farClip = this->_shadow_distance;
	float clipRange = farClip - nearClip;

	float minZ = nearClip;
	float maxZ = nearClip + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	float cascadeSplitLambda = 0.95f;

	for (uint32_t i = 0; i < _shadow_cascades_count; i++) {
		float p = (i + 1) / static_cast<float>(_shadow_cascades_count);
		float log = minZ * pow(ratio, p);
		float uniform = minZ + range * p;
		float d = cascadeSplitLambda * (log - uniform) + uniform;
		_cascade_dists[i] = (d - nearClip) / clipRange;
		_cascade_depths[i] = (nearClip + _cascade_dists[i] * clipRange) * -1.0f;
	}

}

const Color& SceneEnvironmentSettings::GetAmbientColor(){
	return _ambient_color;
}

void SceneEnvironmentSettings::SetAmbientColor(const Color& color){
	_ambient_color = color;
}

float SceneEnvironmentSettings::GetMaxShadowDistance(){
	return _shadow_distance;
}

void SceneEnvironmentSettings::SetMaxShadowDistance(float distance){
	_shadow_distance = distance;
}

uint32 SceneEnvironmentSettings::GetShadowCascadesCount(){
	return _shadow_cascades_count;
}

float* SceneEnvironmentSettings::GetCascadeDepths(){
	return _cascade_depths.data();
}

float* SceneEnvironmentSettings::GetCascadeDists(){
	return _cascade_dists.data();
}

void SceneEnvironmentSettings::SetShadowCascadesCount(uint32 cascades){
	if(cascades > 0 && cascades < 10)
		_shadow_cascades_count = cascades;
}