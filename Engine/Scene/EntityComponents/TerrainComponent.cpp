#include "TerrainComponent.hpp"
#include <Scene/Entity.hpp>
#include <Math/MatrixTransform.hpp>
#include <Core/Random.hpp>
#include <Physics/PhysicsLayer.hpp>
#include <Scene/Scene.hpp>
#include <mpi/Core/Random.hpp>

using namespace VSGE;

TerrainTexturesFactors::TerrainTexturesFactors() {
	_textures_masks = nullptr;
	height = 0;
	width = 0;
	layers = 0;
}
void TerrainTexturesFactors::Allocate(uint32 width, uint32 height, uint32 layers_count) {
	this->height = height;
	this->width = width;
	this->layers = layers;
	uint32 buffer_size = width * height * 4 * layers_count;
	_textures_masks = new byte[buffer_size];
}
void TerrainTexturesFactors::Free() {
	SAFE_RELEASE_ARR(_textures_masks);
}
byte* TerrainTexturesFactors::GetArray() {
	return _textures_masks;
}
byte* TerrainTexturesFactors::GetPointerToCoord(uint32 x, uint32 y, uint32 texture) {
	uint32 layer_num = texture / 4;
	uint32 channel_num = texture % 4;
	uint32 layer_size = width * height * 4;
	return &_textures_masks[layer_num * layer_size + (x * height + y) * 4 + channel_num];
}

byte* TerrainTexturesFactors::GetPointerToCoord(uint32 index, uint32 texture) {
	uint32 layer_num = texture / 4;
	uint32 channel_num = texture % 4;
	uint32 layer_size = width * height * 4;
	return &_textures_masks[layer_num * layer_size + index * 4 + channel_num];
}

void TerrainTexturesFactors::add(uint32 x, uint32 y, uint32 texture_id, uint32 val) {
	byte* ptr = GetPointerToCoord(x, y, texture_id);
	if (static_cast<int>(*ptr) + val <= 255)
		*ptr += val;
	else {
		*ptr = 255;
	}
}
void TerrainTexturesFactors::reduce(uint32 x, uint32 y, uint32 texture_id, uint32 val) {
	byte* ptr = GetPointerToCoord(x, y, texture_id);
	int result = static_cast<int>(*ptr) - val;
	if (result >= 0)
		*ptr -= val;
	else {
		*ptr = 0;
	}
}
void TerrainTexturesFactors::set(uint32 x, uint32 y, uint32 texture_id, uint32 val) {
	byte* ptr = GetPointerToCoord(x, y, texture_id);
	*ptr = val;
}
void TerrainTexturesFactors::set(uint32 index, uint32 texture_id, uint32 val) {
	byte* ptr = GetPointerToCoord(index, texture_id);
	*ptr = val;
}
uint8 TerrainTexturesFactors::get(uint32 index, uint32 texture_id) {
	byte* ptr = GetPointerToCoord(index, texture_id);
	return *ptr;
}

TerrainTexture::TerrainTexture() {
	_roughness_factor = 1.f;
	_metallic_factor = 0.f;
	_height_factor = 0.2f;
}

TerrainGrass::TerrainGrass() {
	_width = 1;
	_height = 1;
}

TerrainComponent::TerrainComponent() {
	_width = 500;
	_length = 500;
	_max_terrain_height = 0;

	_mesh_dirty = false;
	_texturemaps_dirty = false;

	_heightmap = nullptr;
	_vegetables_data = nullptr;

	_heightmap_mesh = nullptr;
	_texture_masks = nullptr;

	indices = nullptr;

	_physics_enabled = true;
	_physical_shape = nullptr;
	_rigidbody = nullptr;

	_cast_shadows = true;

	Flat(0);
	QueueGraphicsUpdate();
}
TerrainComponent::~TerrainComponent() {
	SAFE_RELEASE_ARR(_heightmap);
	SAFE_RELEASE_ARR(indices);
	_texture_factors.Free();
	SAFE_RELEASE_ARR(_vegetables_data)

	SAFE_RELEASE(_heightmap_mesh);
	SAFE_RELEASE(_texture_masks);
}

uint32 TerrainComponent::GetWidth() {
	return _width;
}
uint32 TerrainComponent::GetLength() {
	return _length;
}

void TerrainComponent::SetWidth(uint32 width) {
	_width = width;
}
void TerrainComponent::SetLength(uint32 length) {
	_length = length;
}

uint32 TerrainComponent::GetVerticesCount() {
	return _width * _length;
}

uint32 TerrainComponent::GetIndicesCount() {
	return (_width - 1) * (_length - 1) * 2 * 3;
}

float TerrainComponent::GetMaxTerrainHeight() {
	return _max_terrain_height;
}

Mesh* TerrainComponent::GetTerrainMesh() {
	return _heightmap_mesh;
}

Texture* TerrainComponent::GetTerrainMasksTexture() {
	return _texture_masks;
}

std::vector<TerrainTexture>& TerrainComponent::GetTerrainTextures() {
	return _terrain_textures;
}
std::vector<TerrainGrass>& TerrainComponent::GetTerrainVegetables() {
	return _terrain_grass;
}

const AABB TerrainComponent::GetBoundingBox() {
	AABB result;
	result.PrepareForExtend();
	result.Extend(Vec3(0, 0, 0));
	result.Extend(Vec3(_width, 0, _length));
	return result;
}

std::vector<GrassIdTransforms>& TerrainComponent::GetGrassTransforms() {
	return _grass_transforms;
}

void TerrainComponent::Flat(float height) {
	SAFE_RELEASE_ARR(_heightmap);
	_texture_factors.Free();
	SAFE_RELEASE_ARR(_vegetables_data);

	uint32 vertices_count = GetVerticesCount();

	CreateVerticesArray();
	_texture_factors.Allocate(_width, _length, 16);
	_vegetables_data = new GRASS_ID[vertices_count];

	for (uint32 i = 0; i < vertices_count; i++) {
		_heightmap[i].pos.y = height;
		_vegetables_data[i] = NO_GRASS;
	}
	for (int x = 0; x < _width; x++) {
		for (int y = 0; y < _length; y++) {
			_texture_factors.set(x, y, 0, 255);
		}
	}

	UpdateNormals();
	_texturemaps_dirty = true;
}

void TerrainComponent::ModifyHeight(const Vec2i& position, float height, uint32 range) {
	//Iterate over all pixels
	for (int y = 0; y < _width; y++) {
		for (int x = 0; x < _length; x++) {
			//if pixel is in circle
			float dist = (Vec3(x, y, 0) - Vec3(position.x, position.y, 0)).Length();
			if (dist <= range) {
				//calculate modifier
				float toApply = (height) - static_cast<float>(dist * dist) / (float)range;
				if (toApply > 0) {
					_heightmap[y * _length + x].pos.y += (toApply);
				}
			}
		}
	}
	_mesh_dirty = true;
}

void TerrainComponent::ModifyTexture(const Vec2i& position, float opacity, uint32 range, uint32 texture_id) {
	//Iterate over all pixels
	for (int y = 0; y < _width; y++) {
		for (int x = 0; x < _length; x++) {
			//if pixel is in circle
			float dist = (Vec3(x, y, 0) - Vec3(position.x, position.y, 0)).Length();
			//if vertex is inside circle
			if (dist <= range) {
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
	_texturemaps_dirty = true;
}

void TerrainComponent::ModifyGrass(const Vec2i& position, uint32 range, uint32 grass_id) {
	//Iterate over all pixels
	for (int y = 0; y < _width; y++) {
		for (int x = 0; x < _length; x++) {
			//if pixel is in circle
			float dist = (Vec3(x, y, 0) - Vec3(position.x, position.y, 0)).Length();
			if (dist <= range) {
				_vegetables_data[y * _length + x] = grass_id;
			}
		}
	}
}

Vec2i TerrainComponent::GetRayIntersectionTraingle(const Ray& ray) {
	if (!_heightmap || !indices)
		return Vec2i(-1);

	Mat4 entity_transform = GetEntity()->GetWorldTransform();

	for (uint32 index_i = 0; index_i < GetIndicesCount(); index_i += 3) {
		Vec3 v0 = _heightmap[indices[index_i]].pos;
		Vec3 v1 = _heightmap[indices[index_i + 1]].pos;
		Vec3 v2 = _heightmap[indices[index_i + 2]].pos;

		Vec4 v01 = entity_transform * Vec4(v0, 1);
		Vec4 v11 = entity_transform * Vec4(v1, 1);
		Vec4 v21 = entity_transform * Vec4(v2, 1);

		float dist = 0;
		Vec2 pos;
		bool intersects = ray.IntersectTriangle(v01.Vec3(), v11.Vec3(), v21.Vec3(), dist, pos);
		if (intersects) {
			return Vec2i(v0.z, v0.x);
		}
	}
	return Vec2i(-1);
}

void TerrainComponent::QueueGraphicsUpdate() {
	UpdateVegetables();
}

void TerrainComponent::CreateIndicesArray() {
	SAFE_RELEASE_ARR(indices);
	indices = new uint32[GetIndicesCount()];
	uint32 inds = 0;
	for (uint32 y = 0; y < _length - 1; y++) {
		for (uint32 x = 0; x < _width - 1; x++) {
			indices[inds] = static_cast<uint32>(x * _length + y);
			indices[inds + 2] = static_cast<uint32>(x * _length + _length + y);
			indices[inds + 1] = static_cast<uint32>(x * _length + _length + y + 1);

			indices[inds + 3] = static_cast<uint32>(x * _length + y);
			indices[inds + 5] = static_cast<uint32>(x * _length + _length + y + 1);
			indices[inds + 4] = static_cast<uint32>(x * _length + y + 1);
			//Add inds 6
			inds += 6;
		}
	}
}

void TerrainComponent::CreateVerticesArray() {
	SAFE_RELEASE_ARR(_heightmap)
	_heightmap = new Vertex[GetVerticesCount()];
	for (uint32 y = 0; y < _length; y++) {
		for (uint32 x = 0; x < _width; x++) {
			_heightmap[x * _length + y].uv = Vec2(static_cast<float>(x) / _width, static_cast<float>(y) / _length);
			_heightmap[x * _length + y].pos = Vec3(static_cast<float>(x), 0, static_cast<float>(y));
		}
	}

	CreateIndicesArray();
	UpdateNormals();
	
	_mesh_dirty = true;
}

void TerrainComponent::UpdateMaxHeight() {
	_max_terrain_height = -10000;
	for (uint32 y = 0; y < _length; y++) {
		for (uint32 x = 0; x < _width; x++) {
			if (_max_terrain_height < _heightmap[x * _length + y].pos.y)
				_max_terrain_height = _heightmap[x * _length + y].pos.y;
		}
	}
}

void TerrainComponent::UpdateNormals() {
	CalculateNormals(_heightmap, indices, GetIndicesCount());
	ProcessTangentSpace(_heightmap, indices, GetIndicesCount());
}

void TerrainComponent::UpdateVegetables() {
	float step = 1.0f;

	for (auto& transforms : _grass_transforms) {
		transforms.Clear();
	}

	while ((GRASS_ID)_grass_transforms.size() < _terrain_grass.size()) {
		GrassIdTransforms transforms;
		_grass_transforms.push_back(transforms);
	}

	for (float x = 0.f; x < (float)_width; x += step) {
		for (float y = 0.f; y < (float)_length; y += step) {
			int orig_x = (int)ceil(x);
			int orig_y = (int)ceil(y);

			GRASS_ID vegetable_id = _vegetables_data[orig_x * _length + orig_y];
			if (vegetable_id == NO_GRASS)
				continue;

			auto& grass_transforms = _grass_transforms[vegetable_id];
			auto& grass = _terrain_grass[vegetable_id];

			Vec3 rotation = Vec3(0, Mpi::Random::random(0, 360), 0);
			if (_heightmap) {
				Vec3 normal = _heightmap[orig_x * _length + orig_y].normal;
			}

			Vec3 position = Vec3(x, _heightmap[orig_x * _length + orig_y].pos.y, y);
			Vec3 scale = Vec3(grass._width, grass._height, grass._width);

			Mat4 scale_mat = GetScaleMatrix(scale);
			Mat4 translation_mat = GetTranslationMatrix(position);

			Mat4 transform = scale_mat * GetRotationMatrix(rotation) * translation_mat;
			grass_transforms.AddTransform(transform);
		}
	}
}

void TerrainComponent::UpdateGraphicsMesh() {
	if (!_heightmap_mesh) {
		_heightmap_mesh = Mesh::NewMesh();
	}
	_heightmap_mesh->Destroy();

	_heightmap_mesh->SetVertexBuffer(_heightmap, GetVerticesCount());
	_heightmap_mesh->SetIndexBuffer(indices, GetIndicesCount());
	_heightmap_mesh->Create();
}
void TerrainComponent::UpdateGraphicsTextureMasks() {
	//Update texture masks
	uint32 layers_count = MAX_TEXTURES_PER_TERRAIN / 4;
	if (!_texture_masks) {
		_texture_masks = Texture::NewTexture();
		_texture_masks->Create(_width, _length, FORMAT_RGBA, layers_count, 1);
	}

	uint32 layer_size = _width * _length * 4;
	for (uint32 layer_i = 0; layer_i < layers_count; layer_i++) {
		_texture_masks->AddMipLevel(_texture_factors.GetArray() + layer_i * layer_size, layer_size, _width, _length, 0, layer_i);
	}
	_texture_masks->SetReadyToUseInShaders();
}

void TerrainComponent::AddNewTexture() {
	if (_terrain_textures.size() < MAX_TEXTURES_PER_TERRAIN) {
		TerrainTexture ttexture;
		_terrain_textures.push_back(ttexture);
	}
}

void TerrainComponent::AddNewGrass() {
	TerrainGrass tgrass;
	_terrain_grass.push_back(tgrass);
}

bool TerrainComponent::IsShadowCastEnabled() {
	return _cast_shadows;
}
void TerrainComponent::SetShadowCastEnabled(bool cast) {
	_cast_shadows = cast;
}

void TerrainComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_width);
	serializer.Serialize(_length);
	serializer.Serialize(_cast_shadows);

	uint32 textures_count = _terrain_textures.size();
	uint32 vegetables_count = _terrain_grass.size();

	serializer.Serialize(textures_count);
	serializer.Serialize(vegetables_count);

	for (auto& texture : _terrain_textures) {
		serializer.Serialize(texture._albedo_reference.GetResourceName());
		serializer.Serialize(texture._normal_reference.GetResourceName());
		serializer.Serialize(texture._roughness_reference.GetResourceName());
		serializer.Serialize(texture._metallic_reference.GetResourceName());
		serializer.Serialize(texture._ao_reference.GetResourceName());
		serializer.Serialize(texture._height_reference.GetResourceName());
		//write factors
		serializer.Serialize(texture._roughness_factor);
		serializer.Serialize(texture._metallic_factor);
		serializer.Serialize(texture._height_factor);
	}

	for (auto& grass : _terrain_grass) {
		serializer.Serialize(grass._texture_reference.GetResourceName());
		serializer.Serialize(grass._width);
		serializer.Serialize(grass._height);
	}

	for (uint32 i = 0; i < GetVerticesCount(); i++) {
		serializer.Serialize(_heightmap[i].pos.y);

		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) {
			serializer.Serialize(_texture_factors.get(i, texture_i));
		}

		serializer.Serialize(_vegetables_data[i]);
	}
}
void TerrainComponent::Deserialize(ByteSolver& solver) {
	_width = solver.GetValue<uint32>();
	_length = solver.GetValue<uint32>();
	_cast_shadows = solver.GetValue<bool>();

	Flat(0);

	uint32 textures_count = solver.GetValue<uint32>();
	uint32 vegetables_count = solver.GetValue<uint32>();

	for (uint32 texture_i = 0; texture_i < textures_count; texture_i++) {
		std::string albedo = solver.ReadNextString();
		std::string normal = solver.ReadNextString();
		std::string roughness = solver.ReadNextString();
		std::string metallic = solver.ReadNextString();
		std::string ao = solver.ReadNextString();
		std::string height = solver.ReadNextString();

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

	for (uint32 grass_i = 0; grass_i < vegetables_count; grass_i++) {
		std::string diffuse_resource = solver.ReadNextString();
		float width = solver.GetValue<float>();
		float height = solver.GetValue<float>();

		TerrainGrass grass;
		grass._texture_reference.SetResource(diffuse_resource);
		grass._width = width;
		grass._height = height;
		_terrain_grass.push_back(grass);
	}

	for (uint32 i = 0; i < GetVerticesCount(); i++) {
		_heightmap[i].pos.y = solver.GetValue<float>();

		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) {
			_texture_factors.set(i, texture_i, solver.GetValue<uint8>());
		}

		_vegetables_data[i] = solver.GetValue<GRASS_ID>();
	}

	QueueGraphicsUpdate();
}

void TerrainComponent::OnPreRender() {
	if (_mesh_dirty) {
		UpdateMaxHeight();
		UpdateNormals();
		UpdateGraphicsMesh();
		_mesh_dirty = false;
	}
	if (_texturemaps_dirty) {
		UpdateGraphicsTextureMasks();
		_texturemaps_dirty = false;
	}
}

void TerrainComponent::OnUpdate(){
	if (_physics_enabled) {
		if (!_rigidbody)
			AddPhysicsToWorld();

		if (_rigidbody) {
			btTransform transform = GetEntityBtTransform();
			_rigidbody->setWorldTransform(transform);
		}
	}
}
btTransform TerrainComponent::GetEntityBtTransform() {
	btTransform result;

	Vec3 abs_pos = _entity->GetAbsolutePosition();
	Quat abs_rot = _entity->GetAbsoluteRotation();

	result.setOrigin(btVector3(abs_pos.x, abs_pos.y, abs_pos.z));
	result.setRotation(btQuaternion(abs_rot.x, abs_rot.y, abs_rot.z, abs_rot.w));

	return result;
}
void TerrainComponent::AddPhysicsToWorld() {
	btVector3 local_intertia(0, 0, 0);

	if (_rigidbody)
		_entity->GetScene()->GetPhysicalWorld()->RemoveRigidbody(_rigidbody);

	//release old collider shape
	SAFE_RELEASE(_physical_shape);
	//release old rigidbody
	SAFE_RELEASE(_rigidbody);
	//Create new collision shape
	_physical_shape = GetPhysicalShape();
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
btBvhTriangleMeshShape* TerrainComponent::GetPhysicalShape() {
	int numFaces = (_width - 1) * (_length - 1) * 2;
	int vertStride = sizeof(Vertex);
	int indexStride = 3 * sizeof(uint32);

	btTriangleIndexVertexArray* va = new btTriangleIndexVertexArray(numFaces, (int*)indices, indexStride, _width * _length, reinterpret_cast<btScalar*>(_heightmap), vertStride);
	//Allocate Shape with geometry
	return new btBvhTriangleMeshShape(va, false, true);
}