#pragma once

#include "../Resource.hpp"
#include <Graphics/Mesh.hpp>
#include <vector>

namespace VSGE {
	class MeshResource : public Resource {
	private:
		Mesh* _mesh;
	public:

		MeshResource() {

		}

		void PostLoad(byte* data, uint32 size) {

		}

	};
}