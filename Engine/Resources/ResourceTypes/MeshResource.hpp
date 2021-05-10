#pragma once

#include "../Resource.hpp"
#include <Graphics/Mesh.hpp>
#include <vector>
#include <map>
#include <string>

namespace VSGE {

	class MeshResource;

	class MeshGroupResource : public Resource {
	private:
		bool mPrepared;
		std::vector<MeshResource*> _meshes;
	public:

		MeshGroupResource() {
			mPrepared = false;
		}

		void Prepare();

		ResourceType GetResourceType() {
			return RESOURCE_TYPE_MESHGROUP;
		}

		//void PostLoad(byte* data, uint32 size);

	};

	class MeshResource : public Resource {
	private:
		MeshGroupResource* mGroup;
		Mesh* mesh;
	public:

		MeshResource() {
			mesh = CreateMesh();
			mGroup = nullptr;
		}

		~MeshResource() {
			delete mesh;
		}

		Mesh* GetMesh() {
			return mesh;
		}

		void SetGroup(MeshGroupResource* group) {
			mGroup = group;
		}

		MeshGroupResource* GetGroup() {
			return mGroup;
		}

		ResourceType GetResourceType() {
			return RESOURCE_TYPE_MESH;
		}

		void Load();
	};
}