#pragma once

#include <Math/Vertex.hpp>
#include <Math/AABB.hpp>
#include <Core/VarTypes/Base.hpp>
#include <vector>
#include "GpuObject.hpp"

namespace VSGE {
	struct VertexBufferData {
		byte* mVertices;
		uint32 mVertexSize;
	};
	class Mesh : public IGpuObject {
	protected:
		std::vector<VertexBufferData> mVertexBuffers;
		uint32* mIndexArray;

		AABB mMeshBoundingBox;

		uint32 mVerticesCount;
		uint32 mIndexCount;
	public:
		void SetVertexBuffer(Vertex* vertices, uint32 vertexCount);
		template <typename T>
		void AddVertexBuffer(T* vertices);
		void SetIndexBuffer(uint32* indices, uint32 indicesCount);

		virtual bool Create() = 0;
		virtual void Destroy() = 0;

		uint32 GetVerticesCount() { return mVerticesCount; }
		uint32 GetIndexCount() { return mIndexCount; }
		uint32 GetVertexBuffersCount() { return (uint32)mVertexBuffers.size(); }
		const AABB& GetBoundingBox() { return mMeshBoundingBox; }

		Mesh() : 
			mIndexArray(nullptr),
			mVerticesCount(0),
			mIndexCount(0) 
		{}
	};
}