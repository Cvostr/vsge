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
		uint32* _indexArray;

		AABB _meshBoundingBox;

		uint32 _verticesCount;
		uint32 _indicesCount;
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
		uint32 GetVerticesCount() { return _verticesCount; }
		/// <summary>
		/// Get count of mesh indices
		/// </summary>
		uint32 GetIndexCount() { return _indicesCount; }
		/// <summary>
		/// Get count of mesh indices
		/// </summary>
		/// <returns></returns>
		uint32 GetVertexBuffersCount() { return (uint32)mVertexBuffers.size(); }
		/// <summary>
		/// get aabb of this mesh
		/// </summary>
		const AABB& GetBoundingBox() { return _meshBoundingBox; }

		Mesh() : 
			_indexArray(nullptr),
			_verticesCount(0),
			_indicesCount(0)
		{}
	};

	Mesh* CreateMesh();
}