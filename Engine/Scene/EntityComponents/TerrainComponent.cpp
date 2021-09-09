#include "TerrainComponent.hpp"
#include <Scene/Entity.hpp>
#include <Math/MatrixTransform.hpp>
#include <Core/Random.hpp>

using namespace VSGE;
using namespace YAML;

void TerrainTexturesFactors::add(uint32 texture_id, uint32 val) {
	uint8* ptr = &_textures_factors[texture_id];
	if (static_cast<int>(*ptr) + val <= 255)
		*ptr += val;
	else {
		*ptr = 255;
	}
}
void TerrainTexturesFactors::reduce(uint32 texture_id, uint32 val) {
	uint8* ptr = &_textures_factors[texture_id];
	int result = static_cast<int>(*ptr) - val;
	if (result >= 0)
		*ptr -= val;
	else {
		*ptr = 0;
	}
}

TerrainComponent::TerrainComponent() {
	_width = 500;
	_height = 500;

	_heightmap = nullptr;
	_texture_factors = nullptr;

	_heightmap_mesh = nullptr;
	_texture_masks = nullptr;

	heightmap = nullptr;
	indices = nullptr;

	Flat(0);
	UpdateMesh();
	UpdateTextureMasks();
}
TerrainComponent::~TerrainComponent() {
	SAFE_RELEASE_ARR(_heightmap);
	SAFE_RELEASE_ARR(_texture_factors);
	SAFE_RELEASE_ARR(_vegetables_data)

	SAFE_RELEASE(_heightmap_mesh);
	SAFE_RELEASE(_texture_masks);
}

uint32 TerrainComponent::GetWidth() {
	return _width;
}
uint32 TerrainComponent::GetHeight() {
	return _height;
}

void TerrainComponent::SetWidth(uint32 width) {
	_width = width;
}
void TerrainComponent::SetHeight(uint32 height) {
	_height = height;
}

uint32 TerrainComponent::GetVerticesCount() {
	return _width * _height;
}

uint32 TerrainComponent::GetIndicesCount() {
	return (_width - 1) * (_height - 1) * 2 * 3;
}

Mesh* TerrainComponent::GetTerrainMesh() {
	return _heightmap_mesh;
}

Texture* TerrainComponent::GetTerrainMasksTexture() {
	return _texture_masks;
}

const AABB TerrainComponent::GetBoundingBox() {
	AABB result;
	result.PrepareForExtend();
	result.Extend(Vec3(0, 0, 0));
	result.Extend(Vec3(_width, 0, _height));
	return result;
}

std::vector<GrassIdTransforms>& TerrainComponent::GetGrassTransforms() {
	return _grass_transforms;
}

void TerrainComponent::Flat(float height) {
	SAFE_RELEASE_ARR(_heightmap);
	SAFE_RELEASE_ARR(_texture_factors);

	uint32 vertices_count = GetVerticesCount();

	_heightmap = new float[vertices_count];
	_texture_factors = new TerrainTexturesFactors[vertices_count];
	_vegetables_data = new GRASS_ID[vertices_count];

	for (uint32 i = 0; i < vertices_count; i++) {
		_heightmap[i] = height;
		_texture_factors[i]._textures_factors[0] = 255;
		_vegetables_data[i] = NO_GRASS;
	}
}

void TerrainComponent::ModifyHeight(const Vec2i& position, float height, uint32 range) {
	//Iterate over all pixels
	for (int y = 0; y < _width; y++) {
		for (int x = 0; x < _height; x++) {
			//if pixel is in circle
			float dist = (Vec3(x, y, 0) - Vec3(position.x, position.y, 0)).Length();
			if (dist <= range) {
				//calculate modifier
				float toApply = (height) - static_cast<float>(dist * dist) / (float)range;
				if (toApply > 0) {
					_heightmap[y * _height + x] += (toApply);
				}
			}
		}
	}
}

void TerrainComponent::ModifyTexture(const Vec2i& position, float opacity, uint32 range, uint32 texture_id) {
	//Iterate over all pixels
	for (int y = 0; y < _width; y++) {
		for (int x = 0; x < _height; x++) {
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
						_texture_factors[y * _height + x].reduce(texture_f, _toApply);

				}
				_texture_factors[y * _height + x].add(texture_id, _toApply);
			}
		}
	}
}

void TerrainComponent::ModifyGrass(const Vec2i& position, uint32 range, uint32 grass_id) {
	//Iterate over all pixels
	for (int y = 0; y < _width; y++) {
		for (int x = 0; x < _height; x++) {
			//if pixel is in circle
			float dist = (Vec3(x, y, 0) - Vec3(position.x, position.y, 0)).Length();
			if (dist <= range) {
				_vegetables_data[y * _height + x] = grass_id;
			}
		}
	}
}

Vec2i& TerrainComponent::GetRayIntersectionTraingle(const Ray& ray) {
	if (!heightmap || !indices)
		return Vec2i(-1);

	Mat4 entity_transform = GetEntity()->GetWorldTransform();

	for (uint32 index_i = 0; index_i < GetIndicesCount(); index_i += 3) {
		Vec3 v0 = heightmap[indices[index_i]].pos;
		Vec3 v1 = heightmap[indices[index_i + 1]].pos;
		Vec3 v2 = heightmap[indices[index_i + 2]].pos;

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

void TerrainComponent::UpdateMesh() {
	if (!_heightmap_mesh) {
		_heightmap_mesh = CreateMesh();
	}
	else
		_heightmap_mesh->Destroy();

	SAFE_RELEASE_ARR(heightmap)
	SAFE_RELEASE_ARR(indices)

	heightmap = new Vertex[GetVerticesCount()];
	indices = new uint32[GetIndicesCount()];

	uint32 inds = 0;
	for (uint32 y = 0; y < _height - 1; y++) {
		for (uint32 x = 0; x < _width - 1; x++) {
			indices[inds] = static_cast<uint32>(x * _height + y);
			indices[inds + 2] = static_cast<uint32>(x * _height + _height + y);
			indices[inds + 1] = static_cast<uint32>(x * _height + _height + y + 1);

			indices[inds + 3] = static_cast<uint32>(x * _height + y);
			indices[inds + 5] = static_cast<uint32>(x * _height + _height + y + 1);
			indices[inds + 4] = static_cast<uint32>(x * _height + y + 1);
			//Add inds 6
			inds += 6;
		}
	}

	for (uint32 y = 0; y < _height; y++) {
		for (uint32 x = 0; x < _width; x++) {
			//Set vertex height
			heightmap[x * _height + y].pos = Vec3(static_cast<float>(x), _heightmap[x * _height + y], static_cast<float>(y));
			//Calculate vertex texture UV
			heightmap[x * _height + y].uv = Vec2(static_cast<float>(x) / _width, static_cast<float>(y) / _height);
		}
	}
	//Calculate normals
	for(uint32 ind_i = 0; ind_i < GetIndicesCount() ; ind_i += 3){
        //Get pointer to next terrain vertex
        Vertex* v1 = &heightmap[indices[ind_i]];
        //Get pointers to other vertices of triangle
        Vertex* v2 = &heightmap[indices[ind_i + 1]];
        Vertex* v3 = &heightmap[indices[ind_i + 2]];
        //Poses of other vertices of triangle
        Vec3 v12 = v1->pos - v2->pos;
        Vec3 v13 = v1->pos - v3->pos;
        //Calculate normal
        v1->normal = v12.Cross(v13).GetNormalized();
	}

	ProcessTangentSpace(heightmap, indices, GetIndicesCount());

	_heightmap_mesh->SetVertexBuffer(heightmap, GetVerticesCount());
	_heightmap_mesh->SetIndexBuffer(indices, GetIndicesCount());
	_heightmap_mesh->Create();
}
void TerrainComponent::UpdateTextureMasks() {
	uint32 layers_count = MAX_TEXTURES_PER_TERRAIN / 4;
	if (!_texture_masks) {
		_texture_masks = CreateTexture();
		_texture_masks->Create(_width, _height, FORMAT_RGBA, layers_count, 1);
	}

	for (uint32 layer_i = 0; layer_i < layers_count; layer_i++) {
		uint32 buffer_size = _width * _height * 4;
		byte* mask = new byte[buffer_size];

		for (uint32 y = 0; y < _height; y++) {
			for (uint32 x = 0; x < _width; x++) {
				TerrainTexturesFactors* texture_factors = &_texture_factors[x * _height + y];

				mask[(x * _height + y) * 4] = texture_factors->_textures_factors[layer_i * 4];
				mask[(x * _height + y) * 4 + 1] = texture_factors->_textures_factors[layer_i * 4 + 1];
				mask[(x * _height + y) * 4 + 2] = texture_factors->_textures_factors[layer_i * 4 + 2];
				mask[(x * _height + y) * 4 + 3] = texture_factors->_textures_factors[layer_i * 4 + 3];
			}
		}
		_texture_masks->AddMipLevel(mask, buffer_size, _width, _height, 0, layer_i);
	}

	_texture_masks->CreateImageView();
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
		for (float y = 0.f; y < (float)_height; y += step) {
			int orig_x = (int)ceil(x);
			int orig_y = (int)ceil(y);

			GRASS_ID vegetable_id = _vegetables_data[orig_x * _height + orig_y];
			if (vegetable_id == NO_GRASS)
				continue;

			auto& grass_transforms = _grass_transforms[vegetable_id];
			auto& grass = _terrain_grass[vegetable_id];

			Vec3 rotation = Vec3(0, Random(0, 360), 0);
			if (heightmap) {
				Vec3 normal = heightmap[orig_x * _height + orig_y].normal;
			}

			Vec3 position = Vec3(x, _heightmap[orig_x * _height + orig_y], y);
			Vec3 scale = Vec3(grass._width, grass._height, grass._width);

			Mat4 scale_mat = GetScaleMatrix(scale);
			Mat4 translation_mat = GetTranslationMatrix(position);

			Mat4 transform = scale_mat * GetRotationMatrix(rotation) * translation_mat;
			grass_transforms.AddTransform(transform);
		}
	}
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

void TerrainComponent::Serialize(YAML::Emitter& e) {
	e << Key << "width" << Value << _width;
	e << Key << "height" << Value << _height;

	e << YAML::Key << "textures" << YAML::Value << YAML::BeginSeq;
	for (auto& texture : _terrain_textures) {
		e << BeginMap;
		e << Key << "albedo" << Value << texture._albedo_reference.GetResourceName();
		e << Key << "normal" << Value << texture._normal_reference.GetResourceName();
		e << Key << "roughness" << Value << texture._roughness_reference.GetResourceName();
		e << Key << "metallic" << Value << texture._metallic_reference.GetResourceName();
		e << Key << "ao" << Value << texture._ao_reference.GetResourceName();
		e << Key << "height" << Value << texture._height_reference.GetResourceName();
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
		e << Value << _heightmap[v];
	}
	e << YAML::EndSeq;

	/*e << YAML::Key << "textures_factors" << YAML::Value << YAML::BeginSeq;
	for (uint32 v = 0; v < GetVerticesCount(); v++) {
		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) {
			e << Value << (int)_texture_factors[v]._textures_factors[texture_i];
		}
	}
	e << YAML::EndSeq;*/
}
void TerrainComponent::Deserialize(YAML::Node& entity) {
	_width = entity["width"].as<uint32>();
	_height = entity["height"].as<uint32>();

	Flat(0);

	YAML::Node textures = entity["textures"];
	for (const auto& texture : textures) {
		std::string albedo = texture["albedo"].as<std::string>();
		std::string normal = texture["normal"].as<std::string>();
		std::string roughness = texture["roughness"].as<std::string>();
		std::string metallic = texture["metallic"].as<std::string>();
		std::string ao = texture["ao"].as<std::string>();
		std::string height = texture["height"].as<std::string>();

		TerrainTexture texture;
		texture._albedo_reference.SetResource(albedo);
		texture._normal_reference.SetResource(normal);
		texture._roughness_reference.SetResource(roughness);
		texture._metallic_reference.SetResource(metallic);
		texture._ao_reference.SetResource(ao);
		texture._height_reference.SetResource(height);
		_terrain_textures.push_back(texture);
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
	uint32 v = 0;
	for (const auto& height : heightmap) {
		_heightmap[v++] = height.as<float>();
	}

	/*v = 0;
	YAML::Node textures_factors = entity["textures_factors"];
	for (const auto& factor : textures_factors) {
		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) {
			//_texture_factors[v++]._textures_factors[texture_i] = (uint8)factor.as<int>();
		}
	}*/
	
	UpdateMesh();
	UpdateTextureMasks();
}
void TerrainComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_width);
	serializer.Serialize(_height);

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
	}

	for (auto& grass : _terrain_grass) {
		serializer.Serialize(grass._texture_reference.GetResourceName());
		serializer.Serialize(grass._width);
		serializer.Serialize(grass._height);
	}

	for (uint32 i = 0; i < GetVerticesCount(); i++) {
		serializer.Serialize(_heightmap[i]);

		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) {
			serializer.Serialize(_texture_factors[i]._textures_factors[texture_i]);
		}

		serializer.Serialize(_vegetables_data[i]);
	}
}
void TerrainComponent::Deserialize(ByteSolver& solver) {
	_width = solver.GetValue<uint32>();
	_height = solver.GetValue<uint32>();

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
		_heightmap[i] = solver.GetValue<float>();

		for (uint32 texture_i = 0; texture_i < _terrain_textures.size(); texture_i++) {
			_texture_factors[i]._textures_factors[texture_i] = solver.GetValue<uint8>();
		}

		_vegetables_data[i] = solver.GetValue<GRASS_ID>();
	}

	UpdateMesh();
	UpdateTextureMasks();
	UpdateVegetables();
}