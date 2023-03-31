#pragma once

#include <Math/Vertex.hpp>
#include <Math/AABB.hpp>
#include "Bone.hpp"
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
		std::vector<VertexBufferData> _vertexBuffers;
		uint32* m_indexArray;
		Vec3* _positions_array; //array of vertex positions, useful for physics

		AABB _meshBoundingBox; //Bounding box of mesh
		tBonesList m_bones; //Bones, attached to this mesh

		uint32 m_verticesCount;
		uint32 m_indicesCount;

		Mesh();
	public:

		static Mesh* NewMesh();
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
			_vertexBuffers.push_back(vbd);
		}
		/// <summary>
		/// Set index buffer
		/// </summary>
		/// <param name="indices">- Array of indices</param>
		/// <param name="indicesCount">- Count of indices</param>
		void SetIndexBuffer(uint32* indices, uint32 indicesCount);
		/// <summary>
		/// Create mesh for using graphics API
		/// </summary>
		/// <returns></returns>
		virtual bool Create() = 0;
		/// <summary>
		/// Destroy created mesh
		/// </summary>
		virtual void Destroy() = 0;
		/// <summary>
		/// Get count of mesh vertices
		/// </summary>
		uint32 GetVerticesCount() const;
		/// <summary>
		/// Get count of mesh indices
		/// </summary>
		uint32 GetIndexCount() const;
		/// <summary>
		/// Get count of mesh indices
		/// </summary>
		/// <returns></returns>
		uint32 GetVertexBuffersCount() { return (uint32)_vertexBuffers.size(); }
		/// <summary>
		/// get aabb of this mesh
		/// </summary>
		Vec3* GetPositions() {
			return _positions_array;
		}
		/// <summary>
		/// Get AABB of mesh
		/// </summary>
		/// <returns></returns>
		const AABB& GetBoundingBox() const;

		uint32 GetTrianglesCount();

		void GetTriangle(uint32 triangle_index, Vec3& v0, Vec3& v1, Vec3& v2);

		const tBonesList& GetBones() const;

		void SetBones(Bone* bones, uint32 size);

		virtual ~Mesh();
	};
}