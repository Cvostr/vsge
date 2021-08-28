#include "TerrainComponent.hpp"

using namespace VSGE;

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
}
TerrainComponent::~TerrainComponent() {
	SAFE_RELEASE_ARR(_heightmap);
	SAFE_RELEASE_ARR(_texture_factors);

	SAFE_RELEASE_ARR(_heightmap_mesh);
	SAFE_RELEASE_ARR(_texture_masks);
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

void TerrainComponent::Flat(float height) {
	SAFE_RELEASE_ARR(_heightmap);
	SAFE_RELEASE_ARR(_texture_factors);

	uint32 vertices_count = GetVerticesCount();

	_heightmap = new float[vertices_count];
	_texture_factors = new TerrainTexturesFactors[vertices_count];

	for (uint32 i = 0; i < vertices_count; i++) {
		_heightmap[i] = height;
		_texture_factors[i]._textures_factors[0] = 255;
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

				if (_toApply < 0)
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

Vec2i& TerrainComponent::GetRayIntersectionTraingle(const Ray& ray) {
	for (uint32 index_i = 0; index_i < GetIndicesCount(); index_i += 3) {
		Vec3 v0 = heightmap[indices[index_i]].pos;
		Vec3 v1 = heightmap[indices[index_i + 1]].pos;
		Vec3 v2 = heightmap[indices[index_i + 2]].pos;

		float dist = 0;
		Vec2 pos;
		bool intersects = ray.IntersectTriangle(v0, v1, v2, dist, pos);
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

void TerrainComponent::AddNewTexture() {
	TerrainTexture ttexture;
	_terrain_textures.push_back(ttexture);
}