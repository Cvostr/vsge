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
void VulkanTerrain::SetTerrain(TerrainComponent* terrain) {
	_terrain = terrain;

	if (_terrain->GetTerrainMasksTexture())
		_terrain_descr_set->WriteDescriptorImage(
			1, 
			(VulkanTexture*)_terrain->GetTerrainMasksTexture(),
			VulkanRenderer::Get()->GetTerrainRenderer()->GetTerrainMasksTextureSampler());
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
	_terrain_descr_set->Create();
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
	std::vector<VulkanDescriptorSet*>& descr_set
) {
	this->_gbuffer_renderpass = gbuffer_renderpass;
	this->_entity_descr_set = &descr_set;
	
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
}

void VulkanTerrainRenderer::ProcessTerrain(Entity* terrain) {
	if (_terrains_processed + 1 > _terrains.size()) {
		VulkanTerrain* new_terrain = new VulkanTerrain;
		new_terrain->Create(_terrains_descr_pool);
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