#include "TerrainComponent.hpp"

using namespace VSGE;

TerrainComponent::TerrainComponent() {
	_width = 500;
	_height = 500;

	_heightmap = nullptr;
	_texture_factors = nullptr;

	_heightmap_mesh = nullptr;
	_texture_masks = nullptr;
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

void TerrainComponent::Flat(float height) {
	SAFE_RELEASE_ARR(_heightmap);
	SAFE_RELEASE_ARR(_texture_factors);

	uint32 vertices_count = GetVerticesCount();

	_heightmap = new float[vertices_count];
	_texture_factors = new TerrainTexturesFactors[vertices_count];

	for (uint32 i = 0; i < vertices_count; i++) {
		_heightmap[i] = height;
	}
}
void TerrainComponent::UpdateMesh() {
	if (!_heightmap_mesh) {
		_heightmap_mesh = CreateMesh();
	}

	Vertex* heightmap = new Vertex[GetVerticesCount()];
	uint32* indices = new uint32[GetIndicesCount()];

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
	if (!_texture_masks) {
		uint32 layers_count = MAX_TEXTURES_PER_VERTEX / 4;
		_texture_masks = CreateTexture();
		_texture_masks->Create(_width, _height, FORMAT_RGBA, layers_count, 1);
	}
}