#pragma once

#include "../IEntityComponent.hpp"
#include <Graphics/Mesh.hpp>
#include <Graphics/Texture.hpp>
#include <Math/Ray.hpp>
#include <Resources/ResourceReference.hpp>

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#define MAX_TEXTURES_PER_TERRAIN 16
#define MAX_GRASSES_PER_TERRAIN 200

#define NO_GRASS 255

typedef uint8 GRASS_ID;

namespace VSGE{

	
	class TerrainTexturesFactors {
	private:
		byte* _textures_masks;
		uint32 height;
		uint32 width;
		uint32 layers;
	public:
		TerrainTexturesFactors();
		void Allocate(uint32 width, uint32 height, uint32 layers_count = 16);
		void Free();
		byte* GetArray();
		byte* GetPointerToCoord(uint32 x, uint32 y, uint32 texture);
		byte* GetPointerToCoord(uint32 index, uint32 texture);
		void set(uint32 x, uint32 y, uint32 texture_id, uint32 val);
		void set(uint32 index, uint32 texture_id, uint32 val);
		uint8 get(uint32 index, uint32 texture_id);
		void add(uint32 x, uint32 y, uint32 texture_id, uint32 val);
		void reduce(uint32 x, uint32 y, uint32 texture_id, uint32 val);
	};

	class TerrainTexture {
	public:
		ResourceReference _albedo_reference;
		ResourceReference _normal_reference;
		ResourceReference _roughness_reference;
		ResourceReference _metallic_reference;
		ResourceReference _ao_reference;
		ResourceReference _height_reference;

		float _roughness_factor;
		float _metallic_factor;
		float _height_factor;

		TerrainTexture() {
			_albedo_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_normal_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_roughness_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_metallic_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_ao_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);
			_height_reference.SetResourceType(ResourceType::RESOURCE_TYPE_TEXTURE);

			_roughness_factor = 1.f;
			_metallic_factor = 0.f;
			_height_factor = 0.2f;
		}
	};

	class TerrainGrass {
	public:
		ResourceReference _texture_reference;

		float _width;
		float _height;

		TerrainGrass() {
			_texture_reference.SetResourceType(RESOURCE_TYPE_TEXTURE);
			_width = 1;
			_height = 1;
		}
	};

	class GrassIdTransforms {
	private:
		std::vector<Mat4> _transforms;
	public:

		void Clear() {
			_transforms.clear();
		}

		void AddTransform(const Mat4& transform) {
			_transforms.push_back(transform);
		}

		const std::vector<Mat4>& GetTransforms() {
			return _transforms;
		}
	};

	class TerrainComponent : public IEntityComponent {
		private:
			uint32 _width;
			uint32 _height;
			float _max_terrain_height;
	
			bool _mesh_dirty;
			bool _texturemaps_dirty;

			TerrainTexturesFactors _texture_factors;
			GRASS_ID* _vegetables_data;

			std::vector<TerrainTexture> _terrain_textures;
			std::vector<TerrainGrass> _terrain_grass;
			//graphics api objects
			Mesh* _heightmap_mesh;
			Texture* _texture_masks;
			//computed geometry
			Vertex* _heightmap;
			uint32* indices;
			std::vector<GrassIdTransforms> _grass_transforms;
			//physics
			bool _physics_enabled;
			btBvhTriangleMeshShape* _physical_shape;
			btRigidBody* _rigidbody;
			btTransform GetEntityBtTransform();
			btBvhTriangleMeshShape* GetPhysicalShape();
			void AddPhysicsToWorld();

			void UpdateMaxHeight();
			void UpdateNormals();
			void CreateIndicesArray();
			void CreateVerticesArray();

			void UpdateGraphicsMesh();
			void UpdateGraphicsTextureMasks();
		public:
			TerrainComponent();
			~TerrainComponent();

			uint32 GetWidth();
			uint32 GetHeight();

			void SetWidth(uint32 width);
			void SetHeight(uint32 height);

			uint32 GetVerticesCount();
			uint32 GetIndicesCount();
			float GetMaxTerrainHeight();

			void Flat(float height);
			void ModifyHeight(const Vec2i& position, float height, uint32 range);
			void ModifyTexture(const Vec2i& position, float opacity, uint32 range, uint32 texture_id);
			void ModifyGrass(const Vec2i& position, uint32 range, uint32 grass_id);
			
			void QueueGraphicsUpdate();

			void UpdateVegetables();

			Vec2i GetRayIntersectionTraingle(const Ray& ray);

			std::vector<TerrainTexture>& GetTerrainTextures();
			std::vector<TerrainGrass>& GetTerrainVegetables();

			void AddNewTexture();
			void AddNewGrass();

			Mesh* GetTerrainMesh();
			Texture* GetTerrainMasksTexture();
			std::vector<GrassIdTransforms>& GetGrassTransforms();
			const AABB GetBoundingBox();
			void CreatePhysics();

			void OnPreRender();
			void OnUpdate();

			void Serialize(YAML::Emitter& e);
			void Deserialize(YAML::Node& entity);
			void Serialize(ByteSerialize& serializer);
			void Deserialize(ByteSolver& solver);

			DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_TERRAIN, "Terrain")
	};
}