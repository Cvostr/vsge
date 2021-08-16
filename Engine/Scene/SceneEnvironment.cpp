#include "SceneEnvironment.hpp"

using namespace VSGE;

SceneEnvironmentSettings::SceneEnvironmentSettings(){
	_skybox_material.SetResourceType(RESOURCE_TYPE_MATERIAL);
	_shadow_distance = 200;
	_shadow_cascades_count = 4;
	UpdateShadows();
}

void SceneEnvironmentSettings::UpdateShadows(){
	_cascade_depths.resize(_shadow_cascades_count);

	float step = _shadow_distance / _shadow_cascades_count;
	for (uint32 i = 0; i < _shadow_cascades_count; i++) {

		_cascade_depths[i] = (i + 1) * step * ((float)(i + 1) / _shadow_cascades_count);
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

void SceneEnvironmentSettings::SetShadowCascadesCount(uint32 cascades){
	if(cascades > 0 && cascades < 10)
		_shadow_cascades_count = cascades;
}