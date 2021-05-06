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
		/// <summary>
		/// Set main vertex buffer and calculate AABB
		/// </summary>
		/// <param name="vertices">- Array of vertices</param>
		/// <param name="vertexCount">- Count of vertices</param>
		void SetVertexBuffer(Vertex* vertices, uint32 vertexCount);
		/// <summary>
		/// Add secondary vertex buffer
		/// </summary>
		/// <typeparam name="T">- type of vertex</typeparam>
		/// <param name="vertices">- Array of vertices</param>
		template <class T>
		void AddVertexBuffer(T* vertices) {
			VertexBufferData vbd;
			vbd.mVertices = (byte*)vertices;
			vbd.mVertexSize = sizeof(T);
			mVertexBuffers.push_back(vbd);
		}
		/// <summary>
		/// Set index buffer
		/// </summary>
		/// <param name="indices">- Array of indices</param>
		/// <param name="indicesCount">- Count of indices</param>
		void SetIndexBuffer(uint32* indices, uint32 indicesCount);

		virtual bool Create() = 0;
		virtual void Destroy() = 0;
		/// <summary>
		/// Get count of mesh vertices
		/// </summary>
		uint32 GetVerticesCount() { return mVerticesCount; }
		/// <summary>
		/// Get count of mesh indices
		/// </summary>
		uint32 GetIndexCount() { return mIndexCount; }
		uint32 GetVertexBuffersCount() { return (uint32)mVertexBuffers.size(); }
		const AABB& GetBoundingBox() { return mMeshBoundingBox; }

		Mesh() : 
			mIndexArray(nullptr),
			mVerticesCount(0),
			mIndexCount(0) 
		{}
	};

	Mesh* CreateMesh();
}