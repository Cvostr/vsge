#include "YamlHelper.hpp"

using namespace VSGE;
using namespace YAML;

Emitter& VSGE::operator<<(YAML::Emitter& out, const Guid& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.a << v.b << v.c << v.d << YAML::EndSeq;
	return out;
}

Emitter& VSGE::operator<<(YAML::Emitter& out, const Vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& VSGE::operator<<(YAML::Emitter& out, const Vec2& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

Emitter& VSGE::operator<<(YAML::Emitter& out, const Quat& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

Emitter& VSGE::operator<<(YAML::Emitter& out, const Color& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.r << v.g << v.b << v.a << YAML::EndSeq;
	return out;
}