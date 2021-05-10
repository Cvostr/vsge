#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MeshResource.hpp>
#include <string>

namespace VSGE {
	class MeshComponent : public IEntityComponent {
	private:
		ResourceReference mMeshResource;
	public:
		MeshComponent() {
			mMeshResource.SetResourceType(RESOURCE_TYPE_MESH);
		}
		void SetMeshName(const std::string& mesh);
		MeshResource* GetMeshResource() { return mMeshResource.GetResource<MeshResource>(); }
		ResourceReference& GetResourceReference() {
			return mMeshResource;
		}
	};
}