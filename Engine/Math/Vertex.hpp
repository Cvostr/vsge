#pragma once

#include "Vec3.hpp"
#include "Vec2.hpp"
#include <Core/VarTypes/Base.hpp>

#define MAX_BONE_PER_VERTEX 12

struct Vertex {
	Vec3 pos;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;
	Vec3 bitangent;

	Vertex() {}
	Vertex(Vec3 pos) : pos(pos) {}
	Vertex(Vec3 pos, Vec2 uv) : pos(pos), uv(uv) {}
	Vertex(Vec3 pos, Vec2 uv, Vec3 normal) : pos(pos), uv(uv), normal(normal) {}
	Vertex(Vec3 pos, Vec2 uv, Vec3 normal, Vec3 tangent, Vec3 bitangent) : 
		pos(pos), uv(uv), normal(normal), tangent(tangent), bitangent(bitangent) {}
};

struct VertexSkinningData {
	uint32 ids[MAX_BONE_PER_VERTEX];
	float weights[MAX_BONE_PER_VERTEX];
	uint32 bones_num;

	VertexSkinningData() : bones_num(0) {
		for (uint32 vb_i = 0; vb_i < MAX_BONE_PER_VERTEX; vb_i++) {
			ids[vb_i] = 0;
			weights[vb_i] = 0.0f;
		}
	}
};