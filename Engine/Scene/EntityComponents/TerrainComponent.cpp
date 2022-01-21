#include "TerrainComponent.hpp"
#include <Scene/Entity.hpp>
#include <Math/MatrixTransform.hpp>
#include <Core/Random.hpp>
#include <Physics/PhysicsLayer.hpp>
#include <Scene/Scene.hpp>

using namespace VSGE;
using namespace YAML;

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

TerrainGrass::TerrainGrass() {
	_texture_reference.SetResourceType(RESOURCE_TYPE_TEXTURE);
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

			Vec3 rotation = Vec3(0, Random(0, 360), 0);
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
		_heightmap_mesh = CreateMesh();
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
		_texture_masks = CreateTexture();
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

void TerrainComponent::Serialize(YAML::Emitter& e) {
	e << Key << "width" << Value << _width;
	e << Key << "height" << Value << _length;
	e << Key << "cast_shadows" << Value << _cast_shadows;

	e << YAML::Key << "textures" << YAML::Value << YAML::BeginSeq;
	for (auto& texture : _terrain_textures) {
		e << BeginMap;
		e << Key << "albedo" << Value << texture._albedo_reference.GetResourceName();
		e << Key << "normal" << Value << texture._normal_reference.GetResourceName();
		e << Key << "roughness" << Value << texture._roughness_reference.GetResourceName();
		e << Key << "metallic" << Value << texture._metallic_reference.GetResourceName();
		e << Key << "ao" << Value << texture._ao_reference.GetResourceName();
		e << Key << "height" << Value << texture._height_reference.GetResourceName();
		//write factors
		e << Key << "roughness_factor" << Value << texture._roughness_factor;
		e << Key << "metallic_factor" << Value << texture._metallic_factor;
		e << Key << "height_factor" << Value << texture._height_factor;
		e << YAML::EndMap; // Anim resource
	}
	e << YAML::EndSeq;

	e << YAML::Key << "vegetables" << YAML::Value << YAML::BeginSeq;
	for (auto& grass : _terrain_grass) {
		e << BeginMap;
		e << Key << "diffuse" << Value << grass._texture_reference.GetResourceName();
		e << Key << "width" << Value << grass._width;
		e << Key << "height" << Value << grass._height;
		e << YAML::EndMap; // Anim resource
	}
	e << YAML::EndSeq;

	e << YAML::Key << "heightmap" << YAML::Value << YAML::BeginSeq;
	for (uint32 v = 0; v < GetVerticesCount(); v++) {
		e << Value << _heightmap[v].pos.y;
	}
	for (uint32 v = 0; v < GetVerticesCount(); v++) {
		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) {
			e << Value << (int)_texture_factors.get(v, texture_i);
		}
	}
	for (uint32 v = 0; v < GetVerticesCount(); v++) {
		e << Value << (int)_vegetables_data[v];
	}
	e << YAML::EndSeq;
}
void TerrainComponent::Deserialize(YAML::Node& entity) {
	_width = entity["width"].as<uint32>();
	_length = entity["height"].as<uint32>();
	_cast_shadows = entity["cast_shadows"].as<bool>();

	Flat(0);

	YAML::Node textures = entity["textures"];
	for (const auto& texture : textures) {
		std::string albedo = texture["albedo"].as<std::string>();
		std::string normal = texture["normal"].as<std::string>();
		std::string roughness = texture["roughness"].as<std::string>();
		std::string metallic = texture["metallic"].as<std::string>();
		std::string ao = texture["ao"].as<std::string>();
		std::string height = texture["height"].as<std::string>();
		float roughness_factor = texture["roughness_factor"].as<float>();
		float metallic_factor = texture["metallic_factor"].as<float>();
		float height_factor = texture["height_factor"].as<float>();

		TerrainTexture terrain_texture;
		terrain_texture._albedo_reference.SetResource(albedo);
		terrain_texture._normal_reference.SetResource(normal);
		terrain_texture._roughness_reference.SetResource(roughness);
		terrain_texture._metallic_reference.SetResource(metallic);
		terrain_texture._ao_reference.SetResource(ao);
		terrain_texture._height_reference.SetResource(height);
		terrain_texture._roughness_factor = roughness_factor;
		terrain_texture._metallic_factor = metallic_factor;
		terrain_texture._height_factor = height_factor;

		_terrain_textures.push_back(terrain_texture);
	}

	YAML::Node vegetables = entity["vegetables"];
	for (const auto& vegetable : vegetables) {
		std::string diffuse_resource = vegetable["diffuse"].as<std::string>();
		float width = vegetable["width"].as<float>();
		float height = vegetable["height"].as<float>();

		TerrainGrass grass;
		grass._texture_reference.SetResource(diffuse_resource);
		grass._width = width;
		grass._height = height;
		_terrain_grass.push_back(grass);
	}

	YAML::Node heightmap = entity["heightmap"];
	
	for (uint32 v = 0; v < GetVerticesCount(); v++) {
		_heightmap[v].pos.y = heightmap[v].as<float>();
	}
	for (uint32 v = 0; v < GetVerticesCount(); v++) {
		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) {
			uint8 value = (uint8)heightmap[GetVerticesCount() + _terrain_textures.size() * v + texture_i].as<int>();
			_texture_factors.set(v, texture_i, value);
		}
	}
	for (uint32 v = 0; v < GetVerticesCount(); v++) {
		_vegetables_data[v] = 
			(GRASS_ID)heightmap[GetVerticesCount() + _terrain_textures.size() * GetVerticesCount() + v].as<int>();
	}
	
	UpdateVegetables();
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