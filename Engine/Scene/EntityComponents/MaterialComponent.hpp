#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MaterialResource.hpp>

namespace VSGE {
	class MaterialComponent : public IEntityComponent {
	private:
		ResourceReference _materialResource;
	public:
		MaterialComponent(){
			_materialResource.SetResourceType(RESOURCE_TYPE_MATERIAL);
		}

		MaterialResource* GetMaterialResource() {
			return _materialResource.GetResource<MaterialResource>();
		}

		ResourceReference& GetResourceReference() {
			return _materialResource;
		}

		void SetMaterialName(const std::string& mesh) {
			_materialResource.SetResource(mesh);
		}

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		EntityComponentType GetType() {
			return ENTITY_COMPONENT_MATERIAL;
		}
	};
}