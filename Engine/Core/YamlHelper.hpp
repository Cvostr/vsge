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