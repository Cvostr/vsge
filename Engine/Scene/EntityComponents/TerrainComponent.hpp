#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {

	class TerrainComponent : public IEntityComponent {
	
	public:

		TerrainComponent();
		~TerrainComponent();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_TERRAIN, "Terrain")
	};
}