#include "MeshComponent.hpp"
#include <Resources/ResourceCache.hpp>

using namespace VSGE;

void MeshComponent::SetMeshName(const std::string& mesh) {
	mMeshResource.UpdateResourcePointer(mesh);
	//mMeshResource.mResourcePointer->Load();
}