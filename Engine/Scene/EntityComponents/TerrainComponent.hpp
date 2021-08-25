#pragma once

#include "../IEntityComponent.hpp"
#include <Graphics/Mesh.hpp>
#include <Graphics/Texture.hpp>
#include <Math/Ray.hpp>

#define MAX_TEXTURES_PER_TERRAIN 16

namespace VSGE{

	class TerrainTexturesFactors {
	public:
		uint8 _textures_factors[MAX_TEXTURES_PER_TERRAIN];

		TerrainTexturesFactors() {
			for (uint32 i = 0; i < MAX_TEXTURES_PER_TERRAIN; i++) {
				_textures_factors[i] = 0;
			}
		}
	};

	class TerrainComponent : public IEntityComponent {
		private:
			uint32 _width;
			uint32 _height;
	
			float* _heightmap;
			TerrainTexturesFactors* _texture_factors;

			Mesh* _heightmap_mesh;
			Texture* _texture_masks;

			Vertex* heightmap;
			uint32* indices;
		public:
			TerrainComponent();
			~TerrainComponent();

			uint32 GetWidth();
			uint32 GetHeight();

			void SetWidth(uint32 width);
			void SetHeight(uint32 height);

			uint32 GetVerticesCount();
			uint32 GetIndicesCount();

			void Flat(float height);
			void ModifyHeight(const Vec2i& position, float height, uint32 range);
			void UpdateMesh();
			void UpdateTextureMasks();
			Vec2i& GetRayIntersectionTraingle(const Ray& ray);

			Mesh* GetTerrainMesh();
			Texture* GetTerrainMasksTexture();

			DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_TERRAIN, "Terrain")
	};
}