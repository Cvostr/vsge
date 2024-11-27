#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {

	class TerrainComponent : public IEntityComponent {
	private:
		uint32 _width;
		uint32 _length;
	public:

		TerrainComponent();
		~TerrainComponent();

		uint32 GetWidth() const;
		uint32 GetLength() const;

		uint32 GetVerticesCount() const;
		uint32 GetIndicesCount() const;

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_TERRAIN, "Terrain")
	};
}