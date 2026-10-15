#include "MeshResource.hpp"
#include "../ModelParser.hpp"
#include "../ResourceCache.hpp"

using namespace VSGE;

void MeshGroupResource::Prepare() {
	
	ImportedSceneFile isf;
	isf.loadFromBuffer(_loadedData, _description.size);

	const auto& meshes = isf.getMeshes();

	if (!mPrepared) {
		for (size_t mesh_i = 0; mesh_i < meshes.size(); mesh_i++) {
			MeshContainer* mc = meshes[mesh_i];
			MeshResource* mres = new MeshResource;
			mres->SetName(mc->meshName);
			mres->SetParent(this);
			_subresources.push_back(mres);
			ResourceCache::Get()->PushResource(mres);
		}
			
		mPrepared = true;
	}
	else {
		for (size_t mesh_i = 0; mesh_i < _subresources.size(); mesh_i++) {
			MeshResource* mesh_res = (MeshResource*)_subresources[mesh_i];
			MeshContainer* loadedMesh = meshes[mesh_i];
			if (mesh_res->GetState() == RESOURCE_STATE_UNLOADED) {
				Mesh* mesh = mesh_res->GetMesh();
				mesh->SetVertexBuffer(loadedMesh->vertexArray, loadedMesh->vertexCount);
				mesh->AddVertexBuffer(loadedMesh->vertexSkinningArray);
				mesh->SetIndexBuffer(loadedMesh->indexArray, loadedMesh->indexCount);
				mesh->SetBones(loadedMesh->bonesArray, loadedMesh->bonesCount);
				mesh->Create();
			}
			mesh_res->SetState(RESOURCE_STATE_READY);
		}
	}
		
	SetState(RESOURCE_STATE_UNLOADED);
	FreeLoadedData();
}

void MeshResource::Load() {
	if (m_resourceState == RESOURCE_STATE_UNLOADED) {
		ResourceCache::Get()->GetAsyncLoader()->AddToQueue(m_parent);
	}
}

void MeshResource::OnRelease() {
	if (IsReady())
		mesh->Destroy();
}

Mesh* MeshResource::GetMesh() {
	return mesh;
}