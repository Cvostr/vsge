#include "LightComponent.hpp"
#include <Core/YamlHelper.hpp>
#include "../Entity.hpp"
#include "../Scene.hpp"
#include <Math/MatrixCamera.hpp>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;
using namespace YAML;

template<>
struct YAML::convert<VSGE::Color>
{
	static Node encode(const Color& rhs)
	{
		Node node;
		node.push_back(rhs.r);
		node.push_back(rhs.g);
		node.push_back(rhs.b);
		node.push_back(rhs.a);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, Color& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		rhs.r = node[0].as<float>();
		rhs.g = node[1].as<float>();
		rhs.b = node[2].as<float>();
		rhs.a = node[3].as<float>();
		return true;
	}
};

LightsourceComponent::LightsourceComponent() :
	_intensity(1.f),
	_range(5.5f),
	_spot_angle(12.5f),
	_lightType(LIGHT_TYPE_DIRECTIONAL),

	_castShadows(false),
	_shadowStrength(0.6f),
	_shadowsPCF(4),
	_shadowsBias(0.005f)
{}

LightType& LightsourceComponent::GetLightType()
{ 
	return _lightType; 
}
Color& LightsourceComponent::GetColor() { 
	return _lightColor; 
}

float& LightsourceComponent::GetIntensity() { 
	return _intensity; 
}
float& LightsourceComponent::GetRange() {
	return _range; 
}
float& LightsourceComponent::GetSpotAngle() { 
	return _spot_angle; 
}
bool LightsourceComponent::GetCastShadows() {
	return _castShadows; 
}
void LightsourceComponent::SetCastShadows(bool castShadows) { 
	_castShadows = castShadows; 
}
float LightsourceComponent::GetShadowStrength() {
	return _shadowStrength; 
}
void LightsourceComponent::SetShadowStrength(float strength) {
	_shadowStrength = strength; 
}
float LightsourceComponent::GetShadowsBias() {
	return _shadowsBias;
}
void LightsourceComponent::SetShadowsBias(float bias) {
	_shadowsBias = bias;
}

uint32 LightsourceComponent::GetShadowPCF() {
	return _shadowsPCF;
}
void LightsourceComponent::SetShadowPCF(uint32 pcf) {
	if (pcf < 1)
		pcf = 1;

	_shadowsPCF = pcf;
}

void LightsourceComponent::Serialize(YAML::Emitter& e) {
	e << Key << "light_type" << Value << _lightType;
	e << Key << "intensity" << Value << _intensity;
	e << Key << "range" << Value << _range;
	e << Key << "sp_angle" << Value << _spot_angle;
	e << Key << "color" << Value << _lightColor;
	e << Key << "castShadows" << Value << _castShadows;

	if (_castShadows) {
		e << Key << "shadowStrength" << Value << _shadowStrength;
		e << Key << "shadowBias" << Value << _shadowsBias;
		e << Key << "pcf" << Value << _shadowsPCF;
	}
}
void LightsourceComponent::Deserialize(YAML::Node& entity) {
	_lightType = (LightType)entity["light_type"].as<int>();
	_intensity = entity["intensity"].as<float>();
	_range = entity["range"].as<float>();
	_spot_angle = entity["sp_angle"].as<float>();
	_lightColor = entity["color"].as<Color>();
	_castShadows = entity["castShadows"].as<bool>();

	if (_castShadows) {
		_shadowStrength = entity["shadowStrength"].as<float>();
		_shadowsBias = entity["shadowBias"].as<float>();
		_shadowsPCF = entity["pcf"].as<uint32>();
	}
}

void LightsourceComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_lightType);
	serializer.Serialize(_intensity);
	serializer.Serialize(_range);
	serializer.Serialize(_spot_angle);
	serializer.Serialize(_lightColor);
	serializer.Serialize(_castShadows);

	if (_castShadows) {
		serializer.Serialize(_shadowStrength);
		serializer.Serialize(_shadowsBias);
		serializer.Serialize(_shadowsPCF);
	}
}
void LightsourceComponent::Deserialize(ByteSolver& solver) {
	_lightType = solver.GetValue<LightType>();
	_intensity = solver.GetValue<float>();
	_range = solver.GetValue<float>();
	_spot_angle = solver.GetValue<float>();
	_lightColor = solver.GetValue<Color>();
	_castShadows = solver.GetValue<bool>();

	if (_castShadows) {
		_shadowStrength = solver.GetValue<float>();
		_shadowsBias = solver.GetValue<float>();
		_shadowsPCF = solver.GetValue<uint32>();
	}
}



Mat4* LightsourceComponent::GetShadowcastMatrices(Camera* cam) {
	Vec3 direction = GetDirection();
	Vec3 cam_pos = cam->GetPosition();
	Mat4* result = nullptr;
	if (_castShadows) {
		if (_lightType == LIGHT_TYPE_DIRECTIONAL) {
			float nearClip = 1.0f;
			Scene* scene = GetEntity()->GetScene();
			SceneEnvironmentSettings& env_settings = scene->GetEnvironmentSettings();

			Mat4 invCam = (cam->GetViewMatrix() *
				cam->ComputeOrthoProjectionMatrix(1.0f, env_settings.GetMaxShadowDistance())).invert();

			result = new Mat4[env_settings.GetShadowCascadesCount()];
			for (uint32 i = 0; i < env_settings.GetShadowCascadesCount(); i++) {
				float last_split_dist = (i > 0) ? env_settings.GetCascadeDists()[i - 1] : 0;
				float split_dist = env_settings.GetCascadeDists()[i];

				Vec3 frustumCorners[8] = {
					Vec3(-1.0f,  1.0f, -1.0f),
					Vec3( 1.0f,  1.0f, -1.0f),
					Vec3( 1.0f, -1.0f, -1.0f),
					Vec3(-1.0f, -1.0f, -1.0f),
					Vec3(-1.0f,  1.0f,  1.0f),
					Vec3( 1.0f,  1.0f,  1.0f),
					Vec3( 1.0f, -1.0f,  1.0f),
					Vec3(-1.0f, -1.0f,  1.0f)
				};

				for (uint32_t i = 0; i < 8; i++) {
					Vec4 invCorner = invCam * Vec4(frustumCorners[i], 1.0f);
					frustumCorners[i] = (invCorner / invCorner.w).Vec3();
				}

				for (uint32_t i = 0; i < 4; i++) {
					Vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
					frustumCorners[i + 4] = frustumCorners[i] + (dist * split_dist);
					frustumCorners[i] = frustumCorners[i] + (dist * last_split_dist);
				}

				Vec3 frustumCenter = Vec3(0.0f);
				for (uint32_t i = 0; i < 8; i++) {
					frustumCenter += frustumCorners[i];
				}
				frustumCenter /= 8.0f;

				float radius = 0.0f;
				for (uint32_t i = 0; i < 8; i++) {
					float distance = (frustumCorners[i] - frustumCenter).Length();
					radius = max(radius, distance);
				}
				radius = std::ceil(radius * 16.0f) / 16.0f;

				Vec3 maxExtents = Vec3(radius);
				Vec3 minExtents = maxExtents * -1;

				Vec3 lightDir = (GetDirection() * -1).GetNormalized();
				Vec3 pos = frustumCenter - lightDir * -minExtents.z;
				Mat4 view = GetViewRH(pos, frustumCenter, Vec3(0.0f, 1.0f, 0.0f));
				Mat4 projection = GetOrthoRH_ZeroOne(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, nearClip, maxExtents.z - minExtents.z);
				result[i] = view * projection;
			}
		}
		else if (_lightType == LIGHT_TYPE_POINT) {
			result = new Mat4[6];
			Mat4 projection = GetPerspectiveRH_ZeroOne(90.f, 1, 0.01f, _range);
			projection[1][1] *= -1;
			Vec3 pos = GetEntity()->GetAbsolutePosition();

			result[0] = GetViewRH(pos, pos + Vec3(1, 0, 0), Vec3(0, -1, 0)) * projection;
			result[1] = GetViewRH(pos, pos + Vec3(-1, 0, 0), Vec3(0, -1, 0)) * projection;
			result[2] = GetViewRH(pos, pos + Vec3(0, 1, 0), Vec3(0, 0, 1)) * projection;
			result[3] = GetViewRH(pos, pos + Vec3(0, -1, 0), Vec3(0, 0, -1)) * projection;
			result[4] = GetViewRH(pos, pos + Vec3(0, 0, 1), Vec3(0, -1, 0)) * projection;
			result[5] = GetViewRH(pos, pos + Vec3(0, 0, -1), Vec3(0, -1, 0)) * projection;
		}
		else if (_lightType == LIGHT_TYPE_SPOT) {
			result = new Mat4[1];
			Mat4 projection = GetPerspectiveRH_ZeroOne((_spot_angle + 12) * 2, 1, 0.1f, _range);
			Vec3 pos = GetEntity()->GetAbsolutePosition();
			Vec3 lightDir = (GetDirection()).GetNormalized();

			result[0] = GetViewRH(pos, pos + lightDir, Vec3(0, 1, 0)) * projection;
		}
	}
	return result;
}

Vec3 LightsourceComponent::GetDirection() {
	Quat rotation = _entity->GetAbsoluteRotation();
	return (rotation * Vec3(0, 0, 1)).GetNormalized();
}