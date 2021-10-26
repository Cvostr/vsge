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

Node YAML::convert<Vec2>::encode(const Vec2& rhs)
{
	Node node;
	node.push_back(rhs.x);
	node.push_back(rhs.y);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<Vec2>::decode(const Node& node, Vec2& rhs)
{
	if (!node.IsSequence() || node.size() != 2)
		return false;

	rhs.x = node[0].as<float>();
	rhs.y = node[1].as<float>();
	return true;
}

Node YAML::convert<Vec3>::encode(const Vec3& rhs)
{
	Node node;
	node.push_back(rhs.x);
	node.push_back(rhs.y);
	node.push_back(rhs.z);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<Vec3>::decode(const Node& node, Vec3& rhs)
{
	if (!node.IsSequence() || node.size() != 3)
		return false;

	rhs.x = node[0].as<float>();
	rhs.y = node[1].as<float>();
	rhs.z = node[2].as<float>();
	return true;
}

Node YAML::convert<Quat>::encode(const Quat& rhs)
{
	Node node;
	node.push_back(rhs.x);
	node.push_back(rhs.y);
	node.push_back(rhs.z);
	node.push_back(rhs.w);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<Quat>::decode(const Node& node, Quat& rhs)
{
	if (!node.IsSequence() || node.size() != 4)
		return false;

	rhs.x = node[0].as<float>();
	rhs.y = node[1].as<float>();
	rhs.z = node[2].as<float>();
	rhs.w = node[3].as<float>();
	return true;
}

Node YAML::convert<VSGE::Color>::encode(const Color& rhs)
{
	Node node;
	node.push_back(rhs.r);
	node.push_back(rhs.g);
	node.push_back(rhs.b);
	node.push_back(rhs.a);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<VSGE::Color>::decode(const Node& node, Color& rhs)
{
	if (!node.IsSequence() || node.size() != 4)
		return false;

	rhs.r = node[0].as<float>();
	rhs.g = node[1].as<float>();
	rhs.b = node[2].as<float>();
	rhs.a = node[3].as<float>();
	return true;
}

Node YAML::convert<VSGE::Guid>::encode(const Guid & rhs)
{
	Node node;
	node.push_back(rhs.a);
	node.push_back(rhs.b);
	node.push_back(rhs.c);
	node.push_back(rhs.d);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<VSGE::Guid>::decode(const Node& node, Guid& rhs)
{
	if (!node.IsSequence() || node.size() != 4)
		return false;

	rhs.a = node[0].as<uint32>();
	rhs.b = node[1].as<uint32>();
	rhs.c = node[2].as<uint32>();
	rhs.d = node[3].as<uint32>();
	return true;
}