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

		void Release() {

		}

		void PostLoad() {
			_material->Deserialize(_loadedData, _description.size);
		}

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_MATERIAL)

	};
}