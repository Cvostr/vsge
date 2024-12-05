#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {

	class TerrainComponent : public IEntityComponent {
	private:
		uint32 _width;
		uint32 _length;
		bool _castShadows;

		// state for update
		bool _mesh_dirty;
		bool _texturemaps_dirty;
	public:

		TerrainComponent();
		~TerrainComponent();

		uint32 GetWidth() const;
		uint32 GetLength() const;
		void SetWidth(uint32 width);
		void SetLength(uint32 length);

		uint32 GetVerticesCount() const;
		uint32 GetIndicesCount() const;

		bool IsShadowCastEnabled() const;
		void SetShadowCastEnabled(bool cast);

		void OnUpdate();
		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_TERRAIN, "Terrain")
	};
}