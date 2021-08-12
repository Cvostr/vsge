#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE{
class TerrainComponent : public IEntityComponent {
	private:
		
	public:
		TerrainComponent();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_TERRAIN, "Terrain")
	};
}