#pragma once

#include "../Resource.hpp"
#include <Graphics/Mesh.hpp>

namespace VSGE {

	class MeshResource;

	class MeshGroupResource : public Resource {
	private:
		bool mPrepared;
	public:

		MeshGroupResource() {
			mPrepared = false;
		}

		void Prepare();

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_MESHGROUP)

	};

	class MeshResource : public Resource {
	private:
		Mesh* mesh;
	public:

		MeshResource() {
			mesh = CreateMesh();
		}

		~MeshResource() {
			delete mesh;
		}

		Mesh* GetMesh();

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_MESH)

		void Load();
	};
}