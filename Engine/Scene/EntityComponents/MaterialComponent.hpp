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

		EntityComponentType GetType() {
			return ENTITY_COMPONENT_MATERIAL;
		}
	};
}