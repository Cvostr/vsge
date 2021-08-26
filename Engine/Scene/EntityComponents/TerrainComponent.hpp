#pragma once

#include "../IEntityComponent.hpp"
#include <Graphics/Mesh.hpp>
#include <Graphics/Texture.hpp>
#include <Math/Ray.hpp>
#include <Resources/ResourceReference.hpp>

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

		void add(uint32 texture_id, uint32 val);
		void reduce(uint32 texture_id, uint32 val);
	};

	class TerrainTexture {
	public:
		ResourceReference _albedo_reference;
		ResourceReference _normal_reference;
		ResourceReference _roughness_reference;
		ResourceReference _metallic_reference;
		ResourceReference _ao_reference;
		ResourceReference _height_reference;

		TerrainTexture() {
			_albedo_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_normal_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_roughness_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_metallic_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_ao_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_height_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
		}
	};

	class TerrainComponent : public IEntityComponent {
		private:
			uint32 _width;
			uint32 _height;
	
			float* _heightmap;
			TerrainTexturesFactors* _texture_factors;
			std::vector<TerrainTexture> _terrain_textures;
			//graphics api objects
			Mesh* _heightmap_mesh;
			Texture* _texture_masks;
			//computed geometry
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
			void ModifyTexture(const Vec2i& position, float opacity, uint32 range, uint32 texture_id);
			void UpdateMesh();
			void UpdateTextureMasks();
			Vec2i& GetRayIntersectionTraingle(const Ray& ray);

			std::vector<TerrainTexture>& GetTerrainTextures() {
				return _terrain_textures;
			}
			void AddNewTexture();

			Mesh* GetTerrainMesh();
			Texture* GetTerrainMasksTexture();

			DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_TERRAIN, "Terrain")
	};
}