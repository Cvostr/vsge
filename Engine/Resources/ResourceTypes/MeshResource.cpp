#include "MeshResource.hpp"
#include "../ModelParser.hpp"
#include "../ResourceCache.hpp"

using namespace VSGE;

void MeshGroupResource::Prepare() {
	
	ImportedSceneFile isf;
	isf.loadFromBuffer(mLoadedData, description.size);
	if (!mPrepared) {
		for (uint32 mesh_i = 0; mesh_i < isf.mReadedMeshes.size(); mesh_i++) {
			MeshContainer* mc = isf.mReadedMeshes[mesh_i];
			MeshResource* mres = new MeshResource;
			mres->SetName(mc->meshName);
			mres->SetGroup(this);
			_meshes.push_back(mres);
			ResourceCache::Get()->PushResource(mres);
		}
			
		mPrepared = true;
	}
	else {
		for (uint32 mesh_i = 0; mesh_i < _meshes.size(); mesh_i++) {
			MeshResource* mesh_res = _meshes[mesh_i];
			MeshContainer* loadedMesh = isf.mReadedMeshes[mesh_i];
			if (mesh_res->GetState() == RESOURCE_STATE_UNLOADED) {
				Mesh* mesh = mesh_res->GetMesh();
				mesh->SetVertexBuffer(loadedMesh->vertexArray, loadedMesh->vertexCount);
				mesh->AddVertexBuffer(loadedMesh->vertexSkinningArray);
				mesh->SetIndexBuffer(loadedMesh->indexArray, loadedMesh->indexCount);
				mesh->Create();
			}
			mesh_res->SetState(RESOURCE_STATE_LOADED);
		}
	}
		
	SetState(RESOURCE_STATE_UNLOADED);
	isf.clearMeshes();
	FreeLoadedData();
}

void MeshResource::Load() {
	if (mResourceState == RESOURCE_STATE_UNLOADED) {
		ResourceCache::Get()->GetAsyncLoader()->AddToQueue(mGroup);
	}
}

Mesh* MeshResource::GetMesh() {
	return mesh;
}

void MeshResource::SetGroup(MeshGroupResource* group) {
	mGroup = group;
}

MeshGroupResource* MeshResource::GetGroup() {
	return mGroup;
}

ResourceType MeshResource::GetResourceType() {
	return RESOURCE_TYPE_MESH;
}