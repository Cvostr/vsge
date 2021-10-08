#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MaterialResource.hpp>
#include <Resources/ResourceReference.hpp>

namespace VSGE {
	class MaterialComponent : public IEntityComponent {
	private:
		ResourceReference _materialResource;
		bool _cast_shadows;
	public:
		MaterialComponent();

		MaterialResource* GetMaterialResource();

		ResourceReference& GetResourceReference();

		void SetMaterialName(const std::string& mesh);

		bool IsCastShadows();
		void SetCastShadows(bool cast_shadows);

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_MATERIAL, "Material")
	};
}