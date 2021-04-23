#include "../Mesh.hpp"
#include "VulkanBuffer.hpp"

namespace VSGE {
	class VulkanMesh : public Mesh {
	private:
		VulkanBuffer* vertexBuffer;
		VulkanBuffer* indexBuffer;
	public:

		VulkanMesh() :
			vertexBuffer(nullptr),
			indexBuffer(nullptr) {}

		~VulkanMesh() {
			Destroy();
		}

		void Create(Vertex* vertices, uint32 vertex_count);
		void Create(Vertex* vertices, uint32* indices, uint32 vertex_count, uint32 index_count);
		void Destroy();
	};
}