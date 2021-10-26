#pragma once

#include <yaml-cpp/yaml.h>
#include <Core/VarTypes/Base.hpp>
#include <Core/VarTypes/Guid.hpp>
#include <Math/Color.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Vec3.hpp>
#include <Math/Vec2.hpp>

namespace VSGE {
	YAML::Emitter& operator<<(YAML::Emitter& out, const Guid& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vec3& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vec2& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Quat& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Color& v);
}

template<>
struct YAML::convert<Vec2>
{
	static Node encode(const Vec2& rhs);
	static bool decode(const Node& node, Vec2& rhs);
};

template<>
struct YAML::convert<Vec3>
{
	static Node encode(const Vec3& rhs);
	static bool decode(const Node& node, Vec3& rhs);
};

template<>
struct YAML::convert<Quat>
{
	static Node encode(const Quat& rhs);
	static bool decode(const Node& node, Quat& rhs);
};

template<>
struct YAML::convert<VSGE::Color>
{
	static Node encode(const VSGE::Color& rhs);
	static bool decode(const Node& node, VSGE::Color& rhs);
};

template<>
struct YAML::convert<VSGE::Guid>
{
	static Node encode(const VSGE::Guid& rhs);
	static bool decode(const Node& node, VSGE::Guid& rhs);
};