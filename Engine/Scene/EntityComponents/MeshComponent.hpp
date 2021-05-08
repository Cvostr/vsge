#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MeshResource.hpp>
#include <string>

namespace VSGE {
	class MeshComponent : public IEntityComponent {
	private:
		ResourceReference<MeshResource> mMeshResource;
	public:
		void SetMeshName(const std::string& mesh);
		MeshResource* GetMeshResource() { return mMeshResource.mResourcePointer; }
	};
}