#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MeshResource.hpp>
#include <string>

namespace VSGE {
	class MeshComponent : public IEntityComponent {
	private:
		ResourceReference _meshResource;
	public:
		MeshComponent() {
			_meshResource.SetResourceType(RESOURCE_TYPE_MESH);
		}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_MESH, "Mesh")

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		void SetMeshName(const std::string& mesh);
		MeshResource* GetMeshResource() { return _meshResource.GetResource<MeshResource>(); }
		ResourceReference& GetResourceReference() {
			return _meshResource;
		}
	};
}