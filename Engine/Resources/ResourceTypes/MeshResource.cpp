#include "MeshResource.hpp"
#include "../ModelParser.hpp"
#include "../ResourceCache.hpp"

using namespace VSGE;

void MeshGroupResource::Prepare() {
	
	ImportedSceneFile isf;
	isf.loadFromBuffer(_loadedData, _description.size);
	if (!mPrepared) {
		for (uint32 mesh_i = 0; mesh_i < isf.mReadedMeshes.size(); mesh_i++) {
			MeshContainer* mc = isf.mReadedMeshes[mesh_i];
			MeshResource* mres = new MeshResource;
			mres->SetName(mc->meshName);
			mres->SetParent(this);
			_subresources.push_back(mres);
			ResourceCache::Get()->PushResource(mres);
		}
			
		mPrepared = true;
	}
	else {
		for (uint32 mesh_i = 0; mesh_i < _subresources.size(); mesh_i++) {
			MeshResource* mesh_res = (MeshResource*)_subresources[mesh_i];
			MeshContainer* loadedMesh = isf.mReadedMeshes[mesh_i];
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
	isf.clearMeshes();
	FreeLoadedData();
}

void MeshResource::Load() {
	if (_resourceState == RESOURCE_STATE_UNLOADED) {
		ResourceCache::Get()->GetAsyncLoader()->AddToQueue(_parent);
	}
}

Mesh* MeshResource::GetMesh() {
	return mesh;
}

ResourceType MeshResource::GetResourceType() {
	return RESOURCE_TYPE_MESH;
}