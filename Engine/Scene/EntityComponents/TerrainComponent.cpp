#include "TerrainComponent.hpp"
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <Physics/PhysicsLayer.hpp>

using namespace VSGE;

TerrainTexture::TerrainTexture() {
	_roughness_factor = 1.f;
	_metallic_factor = 0.f;
	_height_factor = 0.2f;
}

TerrainComponent::TerrainComponent():
	_width(256),
	_length(256),
	_castShadows(true),
	_textureTileSize(8, 8),
	// Set graphics objects to null
	_terrain(nullptr),
	_heightmap_mesh(nullptr),
	_texture_masks(nullptr),
	// Intermediate heightmap structs
	_heightmap(nullptr),
	_indices(nullptr),
	// Physics
	_physical_shape(nullptr),
	_rigidbody(nullptr),

	_max_terrain_height(0)
{
	_mesh_dirty = false;
	_texturemaps_dirty = false;

	Flat(0);
	//QueueGraphicsUpdate();
}

TerrainComponent::~TerrainComponent()
{
	if (_rigidbody)
	{
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidbody);
		SAFE_RELEASE(_rigidbody)
	}
	SAFE_RELEASE(_physical_shape)

	SAFE_RELEASE_ARR(_heightmap);
	SAFE_RELEASE_ARR(_indices);
	_texture_factors.Free();
	//SAFE_RELEASE_ARR(_vegetables_data)

	SAFE_RELEASE(_heightmap_mesh);
	SAFE_RELEASE(_texture_masks);
	SAFE_RELEASE(_terrain)
}

uint32 TerrainComponent::GetWidth() const
{
	return _width;
}

uint32 TerrainComponent::GetLength() const
{
	return _length;
}
void TerrainComponent::SetWidth(uint32 width)
{
	_width = width;
}
void TerrainComponent::SetLength(uint32 length)
{
	_length = length;
}

uint32 TerrainComponent::GetVerticesCount() const
{
	return _width * _length;
}

uint32 TerrainComponent::GetIndicesCount() const
{
	return (_width - 1) * (_length - 1) * 2 * 3;
}

bool TerrainComponent::IsShadowCastEnabled() const
{
	return _castShadows;
}

void TerrainComponent::SetTexturesTileSize(const Vec2i& size)
{
	this->_textureTileSize = size;
}

void TerrainComponent::SetShadowCastEnabled(bool cast)
{
	this->_castShadows = cast;
}

const Vec2i& TerrainComponent::GetTexturesTileSize() const
{
	return _textureTileSize;
}

float TerrainComponent::GetMaxTerrainHeight() 
{
	return _max_terrain_height;
}

void TerrainComponent::AddTexture() 
{
	if (_terrain_textures.size() < MAX_TEXTURES_PER_TERRAIN) 
	{
		TerrainTexture ttexture;
		_terrain_textures.push_back(ttexture);
	}
}

void TerrainComponent::SetTerrain(Terrain* terrain)
{
	this->_terrain = terrain;
}

Terrain* TerrainComponent::GetTerrain()
{
	return _terrain;
}

Texture* TerrainComponent::GetTerrainMasksTexture() 
{
	return _texture_masks;
}

Mesh* TerrainComponent::GetTerrainMesh()
{
	return _heightmap_mesh;
}

std::vector<TerrainTexture>& TerrainComponent::GetTerrainTextures() 
{
	return _terrain_textures;
}

const AABB TerrainComponent::GetAABB() {
	AABB result;
	result.PrepareForExtend();
	result.Extend(Vec3(0, 0, 0));
	result.Extend(Vec3(_width, 0, _length));
	return result;
}

void TerrainComponent::OnUpdate()
{
	if (!_rigidbody)
		CreatePhysicsAndAddToWorld();

	if (_rigidbody) 
	{
		btTransform transform = GetEntityBtTransform();
		_rigidbody->setWorldTransform(transform);
	}
}

void TerrainComponent::OnActivate() 
{
	if (_rigidbody) {
		_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_rigidbody);
	}
}

void TerrainComponent::OnDeactivate()
{
	if (_rigidbody) {
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidbody);
	}
}

void TerrainComponent::OnPreRender()
{
	if (_mesh_dirty) 
	{
		UpdateMaxHeight();
		UpdateNormals();
		UpdateGraphicsMesh();
		_mesh_dirty = false;
	}

	if (_texturemaps_dirty) 
	{
		UpdateGraphicsTextureMasks();
		_texturemaps_dirty = false;
	}
}

void TerrainComponent::UpdateMaxHeight() 
{
	_max_terrain_height = -10000;
	for (uint32 y = 0; y < _length; y++) 
	{
		for (uint32 x = 0; x < _width; x++) 
		{
			if (_max_terrain_height < _heightmap[x * _length + y].pos.y)
				_max_terrain_height = _heightmap[x * _length + y].pos.y;
		}
	}
}

void TerrainComponent::CreateIndicesArray()
{
	SAFE_RELEASE_ARR(_indices);
	_indices = new uint32[GetIndicesCount()];
	uint32 inds = 0;

	for (uint32 y = 0; y < _length - 1; y++) 
	{
		for (uint32 x = 0; x < _width - 1; x++) 
		{
			_indices[inds] = static_cast<uint32>(x * _length + y);
			_indices[inds + 2] = static_cast<uint32>(x * _length + _length + y);
			_indices[inds + 1] = static_cast<uint32>(x * _length + _length + y + 1);
			_indices[inds + 3] = static_cast<uint32>(x * _length + y);
			_indices[inds + 5] = static_cast<uint32>(x * _length + _length + y + 1);
			_indices[inds + 4] = static_cast<uint32>(x * _length + y + 1);
			// Add inds 6
			inds += 6;
		}
	}
}

Vec2i TerrainComponent::GetRayIntersectionTraingle(const Ray& ray) 
{
	if (!_heightmap || !_indices)
		return Vec2i(-1);

	Mat4 entity_transform = GetEntity()->GetWorldTransform();

	for (uint32 index_i = 0; index_i < GetIndicesCount(); index_i += 3) 
	{
		Vec3 v0 = _heightmap[_indices[index_i]].pos;
		Vec3 v1 = _heightmap[_indices[index_i + 1]].pos;
		Vec3 v2 = _heightmap[_indices[index_i + 2]].pos;
		Vec4 v01 = entity_transform * Vec4(v0, 1);
		Vec4 v11 = entity_transform * Vec4(v1, 1);
		Vec4 v21 = entity_transform * Vec4(v2, 1);
		
		float dist = 0;
		Vec2 pos;

		bool intersects = ray.IntersectTriangle(v01.Vec3(), v11.Vec3(), v21.Vec3(), dist, pos);
		
		if (intersects) 
		{
			return Vec2i(v0.z, v0.x);
		}
	}

	return Vec2i(-1);
}

void TerrainComponent::Flat(float height) 
{
	SAFE_RELEASE_ARR(_heightmap);
	_texture_factors.Free();
	//SAFE_RELEASE_ARR(_vegetables_data);
	uint32 vertices_count = GetVerticesCount();
	CreateVerticesArray();
	CreateIndicesArray();
	_texture_factors.Allocate(_width, _length, MAX_TEXTURES_PER_TERRAIN);
	//_vegetables_data = new GRASS_ID[vertices_count];
	for (uint32 i = 0; i < vertices_count; i++) {
		_heightmap[i].pos.y = height;
		//_vegetables_data[i] = NO_GRASS;
	}
	for (int x = 0; x < _width; x++) 
	{
		for (int y = 0; y < _length; y++) 
		{
			_texture_factors.set(x, y, 0, 255);
		}
	}

	UpdateNormals();
	_texturemaps_dirty = true;
	_mesh_dirty = true;
}

void TerrainComponent::ModifyHeight(const Vec2i& position, float height, uint32 range) 
{
	//Iterate over all pixels
	for (int y = 0; y < _width; y++) 
	{
		for (int x = 0; x < _length; x++) 
		{
			//if pixel is in circle
			float dist = (Vec3(x, y, 0) - Vec3(position.x, position.y, 0)).Length();
			if (dist <= range) {
				//calculate modifier
				float toApply = (height)-static_cast<float>(dist * dist) / (float)range;
				if (toApply > 0) {
					_heightmap[y * _length + x].pos.y += (toApply);
				}
			}
		}
	}

	_mesh_dirty = true;
}

void TerrainComponent::ModifyTexture(const Vec2i& position, float opacity, uint32 range, uint32 texture_id)
{
	// Iterate over all pixels
	for (int y = 0; y < _width; y++) 
	{
		for (int x = 0; x < _length; x++) 
		{
			//if pixel is in circle
			float dist = (Vec3(x, y, 0) - Vec3(position.x, position.y, 0)).Length();
			//if vertex is inside circle
			if (dist <= range) 
			{
				float toApply = opacity - static_cast<float>(dist * dist) / (float)range;
				int _toApply = (int)(toApply);
				if (_toApply <= 0)
					continue;
				for (uint32 texture_f = 0; texture_f < MAX_TEXTURES_PER_TERRAIN; texture_f++) {
					if (texture_f != texture_id)
						_texture_factors.reduce(y, x, texture_f, _toApply);
				}
				_texture_factors.add(y, x, texture_id, _toApply);
			}
		}
	}
	// enqueue update
	_texturemaps_dirty = true;
}

void TerrainComponent::CreateVerticesArray()
{
	SAFE_RELEASE_ARR(_heightmap)
	_heightmap = new Vertex[GetVerticesCount()];

	for (uint32 y = 0; y < _length; y++) 
	{
		for (uint32 x = 0; x < _width; x++) 
		{
			Vertex* vert = &_heightmap[x * _length + y];
			float fx = static_cast<float>(x);
			float fy = static_cast<float>(y);

			vert->uv = Vec2(fx / _width, fy / _length);
			vert->pos = Vec3(fx, 0, fy);
		}
	}
}

void TerrainComponent::UpdateNormals() 
{
	CalculateNormals(_heightmap, _indices, GetIndicesCount());
	ProcessTangentSpace(_heightmap, _indices, GetIndicesCount());
}

void TerrainComponent::UpdateGraphicsMesh()
{
	if (!_heightmap_mesh) 
	{
		_heightmap_mesh = Mesh::NewMesh();
	}
	else {
		_heightmap_mesh->Destroy();
	}

	_heightmap_mesh->SetVertexBuffer(_heightmap, GetVerticesCount());
	_heightmap_mesh->SetIndexBuffer(_indices, GetIndicesCount());
	_heightmap_mesh->Create();
}

void TerrainComponent::UpdateGraphicsTextureMasks()
{
	uint32 layers_count = MAX_TEXTURES_PER_TERRAIN / 4;

	if (!_texture_masks) 
	{
		// Texture was not created before - then create it
		_texture_masks = Texture::NewTexture();
		_texture_masks->Create(_width, _length, FORMAT_RGBA, layers_count, 1);
	} 
	else if (_texture_masks->GetWidth() != _width || _texture_masks->GetHeight() != _length)
	{
		// Texture was already created, but terrain sizes changed - then recreate it
		_texture_masks->Destroy();
		_texture_masks->Create(_width, _length, FORMAT_RGBA, layers_count, 1);
	}
	
	uint64 layer_size = static_cast<uint64>(_width) * _length * 4;
	for (uint32 layer_i = 0; layer_i < layers_count; layer_i++) 
	{
		_texture_masks->AddMipLevel(_texture_factors.GetArray() + layer_size * layer_i, layer_size, _width, _length, 0, layer_i);
	}

	_texture_masks->SetReadyToUseInShaders();
}

void TerrainComponent::Serialize(ByteSerialize& serializer)
{
	serializer.Serialize(_width);
	serializer.Serialize(_length);
	serializer.Serialize(_castShadows);
	serializer.Serialize(_textureTileSize.x);
	serializer.Serialize(_textureTileSize.y);

	uint32 textures_count = _terrain_textures.size();
	//uint32 vegetables_count = _terrain_grass.size();
	serializer.Serialize(textures_count);
	//serializer.Serialize(vegetables_count);

	for (auto& texture : _terrain_textures) 
	{
		serializer.Serialize(texture._albedo_reference.GetId());
		serializer.Serialize(texture._normal_reference.GetId());
		serializer.Serialize(texture._roughness_reference.GetId());
		serializer.Serialize(texture._metallic_reference.GetId());
		serializer.Serialize(texture._ao_reference.GetId());
		serializer.Serialize(texture._height_reference.GetId());
		//write factors
		serializer.Serialize(texture._roughness_factor);
		serializer.Serialize(texture._metallic_factor);
		serializer.Serialize(texture._height_factor);
	}

	for (uint32 i = 0; i < GetVerticesCount(); i++) 
	{
		serializer.Serialize(_heightmap[i].pos.y);
		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) 
		{
			serializer.Serialize(_texture_factors.get(i, texture_i));
		}
	}
}

void TerrainComponent::Deserialize(ByteSolver& solver)
{
	_width = solver.GetValue<uint32>();
	_length = solver.GetValue<uint32>();
	_castShadows = solver.GetValue<bool>();
	_textureTileSize.x = solver.GetValue<int>();
	_textureTileSize.y = solver.GetValue<int>();

	Flat(0);

	uint32 textures_count = solver.GetValue<uint32>();


	for (uint32 texture_i = 0; texture_i < textures_count; texture_i++) 
	{
		auto albedo = solver.GetGuid();
		auto normal = solver.GetGuid();
		auto roughness = solver.GetGuid();
		auto metallic = solver.GetGuid();
		auto ao = solver.GetGuid();
		auto height = solver.GetGuid();

		TerrainTexture texture;
		texture._albedo_reference.SetResource(albedo);
		texture._normal_reference.SetResource(normal);
		texture._roughness_reference.SetResource(roughness);
		texture._metallic_reference.SetResource(metallic);
		texture._ao_reference.SetResource(ao);
		texture._height_reference.SetResource(height);
		texture._roughness_factor = solver.GetValue<float>();
		texture._metallic_factor = solver.GetValue<float>();
		texture._height_factor = solver.GetValue<float>();
		_terrain_textures.push_back(texture);
	}

	for (uint32 i = 0; i < GetVerticesCount(); i++) 
	{
		_heightmap[i].pos.y = solver.GetValue<float>();
		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) 
		{
			_texture_factors.set(i, texture_i, solver.GetValue<uint8>());
		}
	}
}

void TerrainComponent::CreatePhysicsAndAddToWorld() 
{
	btVector3 local_intertia(0, 0, 0);
	if (_rigidbody)
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidbody);

	//release old collider shape
	SAFE_RELEASE(_physical_shape);
	//release old rigidbody
	SAFE_RELEASE(_rigidbody);
	//Create new collision shape
	_physical_shape = MakePhysicalShape();
	//check new shape
	if (_physical_shape == nullptr)
		//shape wasn't created, exiting
		return;
	btTransform startTransform = GetEntityBtTransform();
	//using motionstate is recommended, itprovides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
	//rigidbody info
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(0, motionState, _physical_shape, local_intertia);
	_rigidbody = new btRigidBody(constructionInfo);
	_rigidbody->setUserPointer(_entity);
	//apply gravity
	_rigidbody->setGravity(btVector3(0, 0, 0));
	//add rigidbody to world
	_entity->GetScene()->GetPhysicalWorld()->AddRigidbody(_rigidbody);
}

btTransform TerrainComponent::GetEntityBtTransform() 
{
	btTransform result;
	Vec3 abs_pos = _entity->GetAbsolutePosition();
	Quat abs_rot = _entity->GetAbsoluteRotation();
	result.setOrigin(btVector3(abs_pos.x, abs_pos.y, abs_pos.z));
	result.setRotation(btQuaternion(abs_rot.x, abs_rot.y, abs_rot.z, abs_rot.w));
	return result;
}

btBvhTriangleMeshShape* TerrainComponent::MakePhysicalShape()
{
	int numFaces = (_width - 1) * (_length - 1) * 2;
	int vertStride = sizeof(Vertex);
	int indexStride = 3 * sizeof(uint32);
	btTriangleIndexVertexArray* va = new btTriangleIndexVertexArray(numFaces, (int*)_indices, indexStride, _width * _length, reinterpret_cast<btScalar*>(_heightmap), vertStride);
	//Allocate Shape with geometry
	return new btBvhTriangleMeshShape(va, false, true);
}