#pragma once

#include "../Resource.hpp"
#include <Graphics/Mesh.hpp>
#include <vector>
#include <map>
#include <string>

namespace VSGE {
	class MeshGroupResource : public Resource {
	private:
		std::vector<std::pair<std::string, Mesh*>> _meshes;
	public:

		MeshGroupResource() {

		}

		void PostLoad(byte* data, uint32 size);

	};
}