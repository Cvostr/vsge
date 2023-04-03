#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MeshResource.hpp>
#include <Resources/ResourceReference.hpp>
#include <string>

namespace VSGE {
	class MeshComponent : public IEntityComponent {
	private:
		ResourceReference _meshResource;
	public:
		MeshComponent() {
		}

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_MESH, "Mesh")

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		void SetMeshId(const Guid& id);
		void SetMesh(const Guid& id, const std::string& childName);
		void SetMeshResource(MeshResource* resource);

		MeshResource* GetMeshResource();
		ResourceReference& GetResourceReference();

		Mesh* GetMesh();
	};
}