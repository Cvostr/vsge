#include "TerrainComponent.hpp"

using namespace VSGE;

TerrainTexturesFactors::TerrainTexturesFactors() 
{
	_textures_masks = nullptr;
	height = 0;
	width = 0;
	layers = 0;
}

TerrainTexturesFactors::~TerrainTexturesFactors()
{
	Free();
}

void TerrainTexturesFactors::Allocate(uint32 width, uint32 height, uint32 layers_count) 
{
	this->height = height;
	this->width = width;
	this->layers = layers;
	uint32 buffer_size = width * height * 4 * layers_count;
	_textures_masks = new byte[buffer_size];
}

void TerrainTexturesFactors::Free()
{
	SAFE_RELEASE_ARR(_textures_masks);
}

byte* TerrainTexturesFactors::GetArray() 
{
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