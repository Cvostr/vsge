#pragma once

#include "../Resource.hpp"
#include <Graphics/Material.hpp>

namespace VSGE {
	class MaterialResource : public Resource {
	private:
		Material* _material;
	public:

		MaterialResource();

		~MaterialResource();

		Material* GetMaterial();

		void OnRelease();

		void PostLoad();

		RESOURCE_CLASS_TYPE(RESOURCE_TYPE_MATERIAL)

	};
}