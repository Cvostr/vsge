#pragma once

#include "../Resource.hpp"
#include <Graphics/Material.hpp>

namespace VSGE {
	class MaterialResource : public Resource {
	private:
		Material* _material;
	public:

		MaterialResource() {
			_material = new Material;
			_material->SetTemplate("default_pbr");
		}

		Material* GetMaterial() { return _material; }

		void PostLoad() {
			//_texture->CreateFromBuffer(mLoadedData, description.size);
		}

		ResourceType GetResourceType() {
			return RESOURCE_TYPE_MATERIAL;
		}

	};
}