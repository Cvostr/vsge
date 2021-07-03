#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MaterialResource.hpp>

namespace VSGE {
	class MaterialComponent : public IEntityComponent {
	private:
		ResourceReference _materialResource;
	public:
		MaterialComponent();

		MaterialResource* GetMaterialResource();

		ResourceReference& GetResourceReference();

		void SetMaterialName(const std::string& mesh);

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		void Serialize(ByteSerialize& e);
		void Deserialize(ByteSolver& solver);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_MATERIAL, "Material")
	};
}