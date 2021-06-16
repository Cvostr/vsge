#include "LightComponent.hpp"
#include "../Entity.hpp"

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

Emitter& operator<<(YAML::Emitter& out, const Color& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.r << v.g << v.b << v.a << YAML::EndSeq;
	return out;
}

void LightsourceComponent::Serialize(YAML::Emitter& e) {
	e << Key << "light_type" << Value << _lightType;
	e << Key << "intensity" << Value << _intensity;
	e << Key << "range" << Value << _range;
	e << Key << "sp_angle" << Value << _spot_angle;
	e << Key << "color" << Value << _lightColor;
}
void LightsourceComponent::Deserialize(YAML::Node& entity) {
	_lightType = (LightType)entity["light_type"].as<int>();
	_intensity = entity["intensity"].as<float>();
	_range = entity["range"].as<float>();
	_spot_angle = entity["sp_angle"].as<float>();
	_lightColor = entity["color"].as<Color>();
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