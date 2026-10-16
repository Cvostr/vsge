#include "VulkanTerrain.hpp"
#include "Rendering/VulkanRenderingGenerics.hpp"
#include <Resources/ResourceTypes/TextureResource.hpp>

using namespace VSGE;

#define TERRAIN_DATA_ELEM_SIZE 512
#define MAX_GRASS_TRANSFORMS 128000u

VulkanTerrainGrass::VulkanTerrainGrass(VulkanDescriptorSet* descrSet) :
	offset(0),
	count(0),
	_descr_set(descrSet)
{

}

VulkanTerrainGrass::VulkanTerrainGrass(VulkanTerrainGrass&& other) noexcept:
	offset(other.offset),
	count(other.count)
{
	this->_descr_set = other._descr_set;
	other._descr_set = nullptr;
}

VulkanTerrainGrass::~VulkanTerrainGrass()
{
	SAFE_RELEASE(_descr_set);
}

VulkanTerrain::VulkanTerrain() :
	info_buffer(nullptr),
	grass_transform_buffer(nullptr),
	_terrain_descr_set(nullptr),
	m_boundComponent(nullptr)
{
}

VulkanTerrain::~VulkanTerrain() 
{
	Destroy();
}

void VulkanTerrain::Destroy()
{
	SAFE_RELEASE(_terrain_descr_set);
	SAFE_RELEASE(info_buffer)
	SAFE_RELEASE(grass_transform_buffer)
}

VulkanDescriptorSet* VulkanTerrain::GetDescriptorSet() 
{
	return _terrain_descr_set;
}
std::vector<VulkanTerrainGrass>& VulkanTerrain::GetVegetables() 
{
	return _grass_descriptor_sets;
}

#define TEXTURE_TYPE_ALBEDO		0
#define TEXTURE_TYPE_NORMAL		1
#define TEXTURE_TYPE_ROUGHNESS	2
#define TEXTURE_TYPE_METALLIC	3
#define TEXTURE_TYPE_AO			4
#define TEXTURE_TYPE_HEIGHT		5

void VulkanTerrain::SetDescriptorTexture(Resource* texture, uint32 texture_type, uint32 texture_index) 
{
	TextureResource* texture_res = static_cast<TextureResource*>(texture);
	if (texture_res == nullptr) {
		//if no texture bound, then bind default texture

		VulkanTexture* default_texture = VulkanRenderingGenerics::Get()->GetBlackTexture();

		if (texture_type == TEXTURE_TYPE_ROUGHNESS || texture_type == TEXTURE_TYPE_METALLIC || texture_type == TEXTURE_TYPE_AO)
		{
			default_texture = VulkanRenderingGenerics::Get()->GetWhiteTexture();
		}

		_terrain_descr_set->WriteDescriptorImage(texture_type + 2,
			default_texture,
			VulkanRenderingGenerics::Get()->GetTerrainTextureSampler(),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			texture_index);
		return;
	}

	if (texture_res->IsUnloaded()) {
		//Load texture
		texture_res->Load();
	}

	if (texture_res->IsReady()) {
		//Mark texture resource as used in this frame
		texture_res->Use();
		//Write texture to descriptor
		_terrain_descr_set->WriteDescriptorImage(texture_type + 2,
			(VulkanTexture*)texture_res->GetTexture(),
			VulkanRenderingGenerics::Get()->GetTerrainTextureSampler(),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			texture_index);
	}
}

void VulkanTerrain::SetDescriptorGrassTexture(Resource* texture, uint32 vegetable_index) {
	TextureResource* texture_res = static_cast<TextureResource*>(texture);
	VulkanDescriptorSet* grass_set = _grass_descriptor_sets[vegetable_index]._descr_set;
	if (texture_res == nullptr) {
		//if no texture bound, then bind default texture
		VulkanTexture* default_texture = VulkanRenderingGenerics::Get()->GetBlackTexture();
		grass_set->WriteDescriptorImage(1,
			default_texture,
			VulkanRenderingGenerics::Get()->GetTerrainTextureSampler(),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		return;
	}

	if (texture_res->IsUnloaded()) {
		//Load texture
		texture_res->Load();
	}

	if (texture_res->IsReady()) {
		//Mark texture resource as used in this frame
		texture_res->Use();
		//Write texture to descriptor
		grass_set->WriteDescriptorImage(1,
			(VulkanTexture*)texture_res->GetTexture(),
			VulkanRenderingGenerics::Get()->GetTerrainTextureSampler(),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

void VulkanTerrain::SetTerrain(TerrainComponent* terrain, VulkanDescriptorPool* pool)
{
	UpdateInfo(terrain);

	if (terrain->GetTerrainMasksTexture())
		_terrain_descr_set->WriteDescriptorImage(
			1,
			(VulkanTexture*)terrain->GetTerrainMasksTexture(),
			VulkanRenderingGenerics::Get()->GetTerrainMasksTextureSampler());

	for (uint32 i = 0; i < terrain->GetTerrainTextures().size(); i++) 
	{
		Resource* albedo_resource = terrain->GetTerrainTextures()[i]._albedo_reference.GetResource();
		Resource* normal_resource = terrain->GetTerrainTextures()[i]._normal_reference.GetResource();
		Resource* roughness_resource = terrain->GetTerrainTextures()[i]._roughness_reference.GetResource();
		Resource* metallic_resource = terrain->GetTerrainTextures()[i]._metallic_reference.GetResource();
		Resource* ao_resource = terrain->GetTerrainTextures()[i]._ao_reference.GetResource();
		Resource* height_resource = terrain->GetTerrainTextures()[i]._height_reference.GetResource();
		//albedo
		SetDescriptorTexture(albedo_resource,
			TEXTURE_TYPE_ALBEDO,
			i);
		//normal
		SetDescriptorTexture(normal_resource,
			TEXTURE_TYPE_NORMAL,
			i);
		//roughness
		SetDescriptorTexture(roughness_resource,
			TEXTURE_TYPE_ROUGHNESS,
			i);
		//metallic
		SetDescriptorTexture(metallic_resource,
			TEXTURE_TYPE_METALLIC,
			i);
		//ao
		SetDescriptorTexture(ao_resource,
			TEXTURE_TYPE_AO,
			i);
		//height
		SetDescriptorTexture(height_resource,
			TEXTURE_TYPE_HEIGHT,
			i);
	}

	// Create vegetables descriptors
	size_t vegetables_count = terrain->GetTerrainVegetables().size();
	while (_grass_descriptor_sets.size() < vegetables_count)
	{
		// Create descriptor set for vegetables transforms
		VulkanDescriptorSet* grass_descr_set = new VulkanDescriptorSet(pool);
		grass_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT); //Transform
		grass_descr_set->Create();
		grass_descr_set->WriteDescriptorBuffer(0, grass_transform_buffer);

		// Construct vulkan object and save it
		VulkanTerrainGrass grass(grass_descr_set);
		_grass_descriptor_sets.push_back(std::move(grass));
	}

	// If vegetable dirty or terrain component changed
	if (terrain->IsVegetablesDirtyAndClear() || m_boundComponent != terrain)
	{
		uint32 veg_written_transforms = 0;

		for (uint32 veg_i = 0; veg_i < vegetables_count; veg_i++)
		{
			auto& grass = terrain->GetTerrainVegetables()[veg_i];

			std::vector<Mat4> veg_transforms;
			grass.GetTransforms(veg_transforms, terrain);

			// how many transforms we gonna write?
			uint32 transforms_count = static_cast<uint32>(veg_transforms.size());
			// protect ourself from overflowing buffer
			transforms_count = std::min(transforms_count, (MAX_GRASS_TRANSFORMS - veg_written_transforms));
			// Write prepared transforms to vulkan buffer
			const Mat4* transforms = veg_transforms.data();
			grass_transform_buffer->WriteData(veg_written_transforms * sizeof(Mat4), transforms_count * sizeof(Mat4), (void*)transforms);


			auto& grass_render_info = _grass_descriptor_sets[veg_i];
			grass_render_info.offset = veg_written_transforms;
			grass_render_info.count = transforms_count;

			// add written transform count
			veg_written_transforms += transforms_count;
		}
	}

	// Update pointer
	m_boundComponent = terrain;
}

void VulkanTerrain::Create(VulkanDescriptorPool* pool) 
{
	_terrain_descr_set = new VulkanDescriptorSet(pool);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 7, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->Create();

	// Allocate buffer for info
	info_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	info_buffer->Create(TERRAIN_DATA_ELEM_SIZE);

	// Allocate buffer for vegetables transforms
	grass_transform_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	grass_transform_buffer->Create(MAX_GRASS_TRANSFORMS * sizeof(Mat4));

	_terrain_descr_set->WriteDescriptorBuffer(0, info_buffer, 0, info_buffer->GetSize());
}

void VulkanTerrain::UpdateInfo(TerrainComponent* terrain)
{
	uint32 offset = 0;

	float uv_x = (float)terrain->GetWidth() / terrain->GetTexturesTileSize().x;
	float uv_y = (float)terrain->GetLength() / terrain->GetTexturesTileSize().y;
	uint32 textures_count = (uint32)terrain->GetTerrainTextures().size();

	info_buffer->WriteData(offset, 4, &uv_x);
	info_buffer->WriteData(offset + 4, 4, &uv_y);
	info_buffer->WriteData(offset + 8, 4, &textures_count);

	offset += 16;
	for (uint32 i = 0; i < terrain->GetTerrainTextures().size(); i++) {
		TerrainTexture* texture = &terrain->GetTerrainTextures()[i];
		info_buffer->WriteData(offset, 4, &texture->_roughness_factor);
		info_buffer->WriteData(offset + 4, 4, &texture->_metallic_factor);
		info_buffer->WriteData(offset + 8, 4, &texture->_height_factor);
		offset += 16;
	}
}

void VulkanTerrain::SetImagesToEmpty() 
{
	for (uint32 i = 0; i < MAX_TEXTURES_PER_TERRAIN; i++) 
	{
		//bind default black texture
		//albedo
		SetDescriptorTexture(nullptr,
			TEXTURE_TYPE_ALBEDO,
			i);
		//normal
		SetDescriptorTexture(nullptr,
			TEXTURE_TYPE_NORMAL,
			i);
		//roughness
		SetDescriptorTexture(nullptr,
			TEXTURE_TYPE_ROUGHNESS,
			i);
		//metallic
		SetDescriptorTexture(nullptr,
			TEXTURE_TYPE_METALLIC,
			i);
		//ao
		SetDescriptorTexture(nullptr,
			TEXTURE_TYPE_AO,
			i);
		//height
		SetDescriptorTexture(nullptr,
			TEXTURE_TYPE_HEIGHT,
			i);

		continue;
	}
}