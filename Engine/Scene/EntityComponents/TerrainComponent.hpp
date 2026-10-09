#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceReference.hpp>

#include <Graphics/Mesh.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Terrain.hpp>
#include <Math/Ray.hpp>

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#define MAX_TEXTURES_PER_TERRAIN 16

namespace VSGE {

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
		TerrainTexture();
		~TerrainTexture() = default;
	};

	class TerrainComponent;

	class TerrainVegetable {
	private:
		uint8* _vegetable_coeff;
	public:
		ResourceReference _mesh_reference;
		ResourceReference _material_reference;
		
		// for mesh itself
		float m_width;
		float m_height;

		void SetWidth(float width);
		void SetHeight(float height);

		uint8* GetCoeffs();

		void Clear(size_t width, size_t height);
		void GetTransforms(std::vector<Mat4> &out, TerrainComponent *terrain);
		TerrainVegetable();
		TerrainVegetable(TerrainVegetable &&);
		~TerrainVegetable();
	};

	class TerrainTexturesFactors {
	private:
		byte* _textures_masks;
		uint32 height;
		uint32 width;
		uint32 layers;
	public:
		TerrainTexturesFactors();
		~TerrainTexturesFactors();
		TerrainTexturesFactors(const TerrainTexturesFactors&) = delete;

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

	class TerrainComponent : public IEntityComponent, GeometricalComponent {
	private:
		uint32 _width;
		uint32 _length;
		bool _castShadows;
		Vec2i _textureTileSize;

		TerrainTexturesFactors _texture_factors;
		std::vector<TerrainTexture> _terrain_textures;

		std::vector<TerrainVegetable> _vegetables;

		// auto-updated value
		float _max_terrain_height;

		// state for update
		bool _mesh_dirty;
		bool _texturemaps_dirty;
		bool _vegetables_dirty;

		// -- Ñomputed geometry state
		Vertex* _heightmap;
		uint32* _indices;

		// -- Graphics Objects
		Mesh* _heightmap_mesh;
		Texture* _texture_masks;

		// Graphics terrain object
		Terrain* _terrain;

		// Physics
		btBvhTriangleMeshShape* _physical_shape;
		btRigidBody* _rigidbody;

		void CreateIndicesArray();
		void CreateVerticesArray();
		void UpdateNormals();

		void UpdateGraphicsTextureMasks();
		void UpdateGraphicsMesh();

		void UpdateMaxHeight();

		// Physics methods
		void CreatePhysicsAndAddToWorld();
		btTransform GetEntityBtTransform();
		btBvhTriangleMeshShape* MakePhysicalShape();
	public:

		TerrainComponent();
		~TerrainComponent();
		TerrainComponent(const TerrainComponent&) = delete;

		uint32 GetWidth() const;
		uint32 GetLength() const;
		void SetWidth(uint32 width);
		void SetLength(uint32 length);

		uint32 GetVerticesCount() const;
		uint32 GetIndicesCount() const;

		bool IsShadowCastEnabled() const;
		void SetShadowCastEnabled(bool cast);

		void SetTexturesTileSize(const Vec2i& size);
		const Vec2i& GetTexturesTileSize() const;

		float GetMaxTerrainHeight();
		void AddTexture();
		void Flat(float height);

		void ModifyHeight(const Vec2i& position, float height, uint32 range, bool up = true);
		void ModifyTexture(const Vec2i& position, float opacity, uint32 range, uint32 texture_id);
		void ModifyVegetable(const Vec2i& position, uint32 range, uint32 grass_id);

		bool IsVegetablesDirtyAndClear();
		void SetVegetablesDirty();

		Vec2i GetRayIntersectionTraingle(const Ray& ray);
		const AABB GetAABB();

		void SetTerrain(Terrain* terrain);
		Terrain* GetTerrain();
		Texture* GetTerrainMasksTexture();
		Mesh* GetTerrainMesh();
		Vertex* GetVertices();

		std::vector<TerrainTexture>& GetTerrainTextures();
		std::vector<TerrainVegetable>& GetTerrainVegetables();

		void OnUpdate();
		void OnPreRender();
		void OnActivate();
		void OnDeactivate();

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_TERRAIN, "Terrain")
	};
}