#pragma once

#include "Vec3.hpp"
#include "Vec2.hpp"

struct Vertex {
	Vec3 pos;
	Vec2 uv;
	Vec3 normal;

	Vertex(Vec3 pos) : pos(pos) {}
	Vertex(Vec3 pos, Vec2 uv) : pos(pos), uv(uv) {}
	Vertex(Vec3 pos, Vec2 uv, Vec3 normal) : pos(pos), uv(uv), normal(normal) {}
};