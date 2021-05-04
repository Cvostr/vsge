#include "MeshResource.hpp"
#include "../ModelParser.hpp"

using namespace VSGE;

void MeshGroupResource::PostLoad(byte* data, uint32 size) {
	ImportedSceneFile isf;
	isf.loadFromBuffer(data, size);

	for (uint32 mesh_i = 0; mesh_i < isf.mReadedMeshes.size(); mesh_i++) {
		MeshContainer* mc = isf.mReadedMeshes[mesh_i];
		Mesh* mesh = CreateMesh();
		std::pair ms_pair = std::pair<std::string, Mesh*>(mc->meshName, mesh);
		_meshes.push_back(ms_pair);
	}

	isf.clearMeshes();
}