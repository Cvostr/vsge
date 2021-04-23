#pragma once

#include <Math/Vertex.hpp>
#include <Math/AABB.hpp>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	class Mesh {
	protected:
		AABB mMeshBoundingBox;

		uint32 mVerticesCount;
		uint32 mIndexCount;
	public:
		virtual void Create(Vertex* vertices, uint32 vertex_count) = 0;
		virtual void Create(Vertex* vertices, uint32* indices, uint32 vertex_count, uint32 index_count) = 0;

		uint32 GetVerticesCount() { return mVerticesCount; }
		uint32 GetIndexCount() { return mIndexCount; }
		const AABB& GetBoundingBox() { return mMeshBoundingBox; }

		Mesh() : 
			mVerticesCount(0),
			mIndexCount(0) 
		{}
	};
}