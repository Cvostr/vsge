#include "VulkanRenderer.hpp"
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Scene/Entity.hpp>

using namespace VSGE;

VulkanTerrain::VulkanTerrain() {
	_terrain_descr_set = nullptr;
	_terrain = nullptr;
}
VulkanTerrain::~VulkanTerrain() {
	SAFE_RELEASE(_terrain_descr_set);
}
VulkanDescriptorSet* VulkanTerrain::GetDescriptorSet() {
	return _terrain_descr_set;
}

void VulkanTerrain::SetDescriptorTexture(Resource* texture, uint32 texture_type, uint32 texture_index) {
	TextureResource* texture_res = static_cast<TextureResource*>(texture);
	if (texture_res == nullptr) {
		//if no texture bound, then bind default white texture
		_terrain_descr_set->WriteDescriptorImage(texture_type + 2,
			VulkanRenderer::Get()->GetTerrainRenderer()->GetEmptyTexture(),
			VulkanRenderer::Get()->GetTerrainRenderer()->GetTerrainTextureSampler(),
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
			VulkanRenderer::Get()->GetTerrainRenderer()->GetTerrainTextureSampler(),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			texture_index);
	}
}

void VulkanTerrain::SetTerrain(TerrainComponent* terrain) {
	_terrain = terrain;

	if (_terrain->GetTerrainMasksTexture())
		_terrain_descr_set->WriteDescriptorImage(
			1, 
			(VulkanTexture*)_terrain->GetTerrainMasksTexture(),
			VulkanRenderer::Get()->GetTerrainRenderer()->GetTerrainMasksTextureSampler());

	for (uint32 i = 0; i < terrain->GetTerrainTextures().size(); i++) {
		Resource* albedo_resource = terrain->GetTerrainTextures()[i]._albedo_reference.GetResource();
		Resource* normal_resource = terrain->GetTerrainTextures()[i]._normal_reference.GetResource();
		Resource* roughness_resource = terrain->GetTerrainTextures()[i]._roughness_reference.GetResource();
		Resource* metallic_resource = terrain->GetTerrainTextures()[i]._metallic_reference.GetResource();
		Resource* ao_resource = terrain->GetTerrainTextures()[i]._ao_reference.GetResource();
		Resource* height_resource = terrain->GetTerrainTextures()[i]._height_reference.GetResource();
		//albedo
		SetDescriptorTexture(albedo_resource,
			0,
			i);
		//normal
		SetDescriptorTexture(normal_resource,
			1,
			i);
		//roughness
		SetDescriptorTexture(roughness_resource,
			2,
			i);
		//metallic
		SetDescriptorTexture(metallic_resource,
			3,
			i);
		//ao
		SetDescriptorTexture(ao_resource,
			4,
			i);
		//height
		SetDescriptorTexture(height_resource,
			5,
			i);
	}
}
TerrainComponent* VulkanTerrain::GetTerrain() {
	return _terrain;
}
void VulkanTerrain::Create(VulkanDescriptorPool* pool) {
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
}

void VulkanTerrain::SetImagesToEmpty() {
	for (uint32 i = 0; i < MAX_TEXTURES_PER_TERRAIN; i++) {
		//bind default black texture
		//albedo
		SetDescriptorTexture(nullptr,
			0,
			i);
		//normal
		SetDescriptorTexture(nullptr,
			1,
			i);
		//roughness
		SetDescriptorTexture(nullptr,
			2,
			i);
		//metallic
		SetDescriptorTexture(nullptr,
			3,
			i);
		//ao
		SetDescriptorTexture(nullptr,
			4,
			i);
		//height
		SetDescriptorTexture(nullptr,
			5,
			i);

		continue;
	}
}

void VulkanTerrain::Destroy() {
	delete _terrain_descr_set;
}

VulkanTerrainRenderer::VulkanTerrainRenderer() {
	_terrains_processed = 0;
}
VulkanTerrainRenderer::~VulkanTerrainRenderer() {

}

void VulkanTerrainRenderer::Create(
	VulkanRenderPass* gbuffer_renderpass,
	std::vector<VulkanDescriptorSet*>& descr_set,
	VulkanTexture* emptyTexture
) {
	this->_gbuffer_renderpass = gbuffer_renderpass;
	this->_entity_descr_set = &descr_set;
	this->_emptyTexture = emptyTexture;
	
	_terrains_descr_pool = new VulkanDescriptorPool;
	_terrains_descr_pool->SetDescriptorSetsCount(1000);
	_terrains_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000);
	_terrains_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5000);
	_terrains_descr_pool->Create();

	_terrain_shader = new VulkanShader;
	_terrain_shader->AddShaderFromFile("terrain.vert", SHADER_STAGE_VERTEX);
	_terrain_shader->AddShaderFromFile("terrain.frag", SHADER_STAGE_FRAGMENT);

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);
	_vertexLayout.AddItem(2, offsetof(Vertex, normal), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(3, offsetof(Vertex, tangent), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(4, offsetof(Vertex, bitangent), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);

	VulkanTerrain* sample_terrain = new VulkanTerrain;
	sample_terrain->Create(_terrains_descr_pool);

	_terrain_pipeline_layout = new VulkanPipelineLayout;
	_terrain_pipeline_layout->PushDescriptorSet(_entity_descr_set->at(0));
	_terrain_pipeline_layout->PushDescriptorSet(sample_terrain->GetDescriptorSet());
	_terrain_pipeline_layout->Create();

	delete sample_terrain;

	_terrain_pipeline = new VulkanPipeline;
	_terrain_pipeline->SetDepthTest(true);
	_terrain_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_terrain_pipeline->Create(_terrain_shader, gbuffer_renderpass, _vertexLayout, _terrain_pipeline_layout);

	_terrain_masks_sampler = new VulkanSampler;
	_terrain_masks_sampler->Create();

	_terrain_textures_sampler = new VulkanSampler;
	_terrain_textures_sampler->SetWrapModes(SAMPLER_WRAP_MIRRORED_REPEAT, SAMPLER_WRAP_MIRRORED_REPEAT);
	_terrain_textures_sampler->Create();
}

void VulkanTerrainRenderer::ProcessTerrain(Entity* terrain) {
	if (_terrains_processed + 1 > _terrains.size()) {
		VulkanTerrain* new_terrain = new VulkanTerrain;
		new_terrain->Create(_terrains_descr_pool);
		new_terrain->SetImagesToEmpty();
		_terrains.push_back(new_terrain);
	}
	VulkanTerrain* vk_terrain = _terrains[_terrains_processed];
	vk_terrain->SetTerrain(terrain->GetComponent<TerrainComponent>());

	_terrains_processed++;
}

void VulkanTerrainRenderer::ResetProcessedTerrains() {
	_terrains_processed = 0;
}

void VulkanTerrainRenderer::SetOutputSizes(uint32 width, uint32 height) {
	_outputWidth = width;
	_outputHeight = height;
}

VulkanSampler* VulkanTerrainRenderer::GetTerrainMasksTextureSampler() {
	return _terrain_masks_sampler;
}

VulkanSampler* VulkanTerrainRenderer::GetTerrainTextureSampler() {
	return _terrain_textures_sampler;
}

VulkanTexture* VulkanTerrainRenderer::GetEmptyTexture() {
	return _emptyTexture;
}

void VulkanTerrainRenderer::DrawTerrain(VulkanCommandBuffer* cmdbuffer, uint32 terrain_index, uint32 draw_index) {
	VulkanTerrain* vk_terrain = _terrains[terrain_index];
	TerrainComponent* terrain = vk_terrain->GetTerrain();

	VulkanMesh* terrain_mesh = (VulkanMesh*)terrain->GetTerrainMesh();
	if (terrain_mesh) {
		cmdbuffer->BindPipeline(*_terrain_pipeline);

		uint32 offsets[2] = { 0, draw_index * UNI_ALIGN % 65535 };
		int vertexDescriptorID = (draw_index * UNI_ALIGN) / 65535;
		cmdbuffer->SetViewport(0, 0, _outputWidth, _outputHeight);

		cmdbuffer->BindDescriptorSets(*_terrain_pipeline_layout, 0, 1, _entity_descr_set->at(vertexDescriptorID), 2, offsets);
		cmdbuffer->BindDescriptorSets(*_terrain_pipeline_layout, 1, 1, vk_terrain->GetDescriptorSet());

		cmdbuffer->BindMesh(*terrain_mesh);
		cmdbuffer->DrawIndexed(terrain->GetIndicesCount());
	}
}