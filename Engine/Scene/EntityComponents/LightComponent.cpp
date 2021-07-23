#include "LightComponent.hpp"
#include <Core/YamlHelper.hpp>
#include "../Entity.hpp"
#include <Math/MatrixCamera.hpp>

using namespace VSGE;
using namespace YAML;

template<>
struct convert<VSGE::Color>
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
	_shadowsCascadesCount(3),
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
void LightsourceComponent::SetShadowCascadesCount(uint32 cascades) {
	if (cascades > MAX_SHADOW_CASCADES)
		cascades = MAX_SHADOW_CASCADES;

	if (cascades < 1)
		cascades = 1;

	_shadowsCascadesCount = cascades;
}
uint32 LightsourceComponent::GetShadowCascadesCount() {
	return _shadowsCascadesCount;
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
		e << Key << "cascades" << Value << _shadowsCascadesCount;
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
		_shadowsCascadesCount = entity["cascades"].as<uint32>();
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
		serializer.Serialize(_shadowsCascadesCount);
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
		_shadowsCascadesCount = solver.GetValue<uint32>();
	}
}

Mat4* LightsourceComponent::GetShadowcastMatrices(Camera* cam) {
	int sizes[] = { 20, 50, 80, 130, 160, 190 };
	Vec3 direction = GetDirection();

	Mat4* result = nullptr;
	if (_castShadows) {
		result = new Mat4[_shadowsCascadesCount];
		for (uint32 i = 0; i < _shadowsCascadesCount; i++) {

			Vec3 cam_pos = cam->GetPosition() + cam->GetFront() * static_cast<float>(sizes[i]);
			Mat4 matview = GetViewRH(cam_pos, cam_pos + direction, Vec3(0, 1, 0));

			float w = sizes[i];
			Mat4 projectionMat = GetOrthoRH_ZeroOne(-w, w, -w, w, 0.1f, 100.f);
			projectionMat.Values[1][1] *= -1;

			result[i] = matview * projectionMat;
		}
	}
	return result;
}

Vec3 LightsourceComponent::GetDirection() {
	Vec3 euler_rotation = _entity->GetAbsoluteRotation().GetEulerAngles();
	euler_rotation *= (3.14159265f / 180.f);

	float yaw = euler_rotation.y;
	float pitch = euler_rotation.x;
	float roll = euler_rotation.z;

	Vec3 result;
	float cy = cosf(yaw * 0.5f);
	float sy = sinf(yaw * 0.5f);
	float cr = cosf(roll * 0.5f);
	float sr = sinf(roll * 0.5f);
	float cp = cosf(pitch * 0.5f);
	float sp = sinf(pitch * 0.5f);

	result.z = (cy * sr * cp - sy * cr * sp);
	result.x = (cy * cr * sp + sy * sr * cp);
	result.y = (sy * cr * cp - cy * sr * sp);

	return result.GetNormalized();
	//Quat rotation = _entity->GetAbsoluteRotation();
	//return Vec3(rotation.x, rotation.y, rotation.z).GetNormalized();
}