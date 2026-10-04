#include "VulkanRenderer.hpp"
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Scene/Entity.hpp>
#include "VulkanRenderingGenerics.hpp"
#include "../VulkanRAPI.hpp"

using namespace VSGE;

static Vertex grass_vertices[] = {
	// back face
	Vertex(Vec3(-0.5f, 0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec3(0.0f,  0.0f, -1.0f)), // bottom-left
	Vertex(Vec3(0.5f, 0.0f, 0.0f), Vec2(1.0f, 1.0f), Vec3(0.0f,  0.0f, -1.0f)), // top-right
	Vertex(Vec3(0.5f, 0.5f, 0.0f), Vec2(1.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f)), // bottom-right
	Vertex(Vec3(-0.5f, 0.5f, 0.0f), Vec2(0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f)), // top-right
	Vertex(Vec3(0.0f, 0.0f, 0.5f), Vec2(1.0f, 1.0f), Vec3(0.0f,  0.0f, -1.0f)), // bottom-left
	Vertex(Vec3(0.0f, 0.0f, -0.5f), Vec2(0.0f, 1.0f), Vec3(0.0f,  0.0f, -1.0f)), // top-left
	Vertex(Vec3(0.0f, 0.5f, 0.5f), Vec2(1.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f)), // bottom-left
	Vertex(Vec3(0.0f, 0.5f, -0.5f), Vec2(0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f)), // bottom-right
};
static uint32 grass_ind[] = { 0,1,2,2,1,0,  0,2,3,3,2,0,   4,5,7,7,5,4,  4,7,6,6,7,4 };

bool TerrainPipelinesInitialized = false;
static VulkanPipelineLayout* terrain_pipeline_layout = nullptr;
static VulkanPipeline* terrain_pipeline = nullptr;
static VulkanPipelineLayout* grass_pipeline_layout = nullptr;
static VulkanPipeline* grass_pipeline = nullptr;
static VulkanMesh* terrainGrassMesh = nullptr;

void InitTerrainPipelines(VulkanDescriptorSet* vertexSet, VulkanDescriptorPool* pool, VulkanRenderPass* gbuffer_renderpass)
{
	CalculateNormals(grass_vertices, grass_ind, 24);
	ProcessTangentSpace(grass_vertices, grass_ind, 24);

	// Fill vertex layout
	VertexLayout terrainVertexLayout;
	terrainVertexLayout.AddBinding(sizeof(Vertex));
	terrainVertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	terrainVertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);
	terrainVertexLayout.AddItem(2, offsetof(Vertex, normal), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	terrainVertexLayout.AddItem(3, offsetof(Vertex, tangent), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	terrainVertexLayout.AddItem(4, offsetof(Vertex, bitangent), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);

	if (terrain_pipeline_layout == nullptr) 
	{
		VulkanTerrain* sample_terrain = new VulkanTerrain;
		sample_terrain->Create(pool);

		terrain_pipeline_layout = new VulkanPipelineLayout;
		terrain_pipeline_layout->PushDescriptorSet(vertexSet);
		terrain_pipeline_layout->PushDescriptorSet(sample_terrain->GetDescriptorSet());
		terrain_pipeline_layout->Create();

		delete sample_terrain;
	}

	if (terrain_pipeline == nullptr)
	{
		VulkanShader* terrain_shader = (VulkanShader*)ShaderCache::Get()->GetShader("terrain");

		terrain_pipeline = new VulkanPipeline;
		terrain_pipeline->SetDepthTest(true);
		terrain_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
		terrain_pipeline->Create(terrain_shader, gbuffer_renderpass, terrainVertexLayout, terrain_pipeline_layout);
	}

	if (terrainGrassMesh == nullptr)
	{
		terrainGrassMesh = new VulkanMesh;
		terrainGrassMesh->SetVertexBuffer(grass_vertices, 8);
		terrainGrassMesh->SetIndexBuffer(grass_ind, 24);
		terrainGrassMesh->Create();
	}

	if (grass_pipeline_layout == nullptr)
	{
		VulkanDescriptorSet* grass_descr_set = new VulkanDescriptorSet(pool);
		grass_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT);
		grass_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
		grass_descr_set->Create();
		grass_pipeline_layout = new VulkanPipelineLayout;
		grass_pipeline_layout->PushDescriptorSet(vertexSet);
		grass_pipeline_layout->PushDescriptorSet(grass_descr_set);
		grass_pipeline_layout->Create();
		delete grass_descr_set;
	}

	if (grass_pipeline == nullptr)
	{
		VulkanShader* grass_shader = (VulkanShader*)ShaderCache::Get()->GetShader("grass");
		grass_pipeline = new VulkanPipeline;
		grass_pipeline->SetDepthTest(true);
		grass_pipeline->Create(grass_shader, gbuffer_renderpass, terrainVertexLayout, grass_pipeline_layout);
	}
}

VulkanTerrainRenderer::VulkanTerrainRenderer() :
	_terrains_descr_pool(nullptr),
	_vegetables_descr_pool(nullptr)
{
	_terrains_processed = 0;
	_vegetables_transforms_written = 0;
}
VulkanTerrainRenderer::~VulkanTerrainRenderer() 
{
	SAFE_RELEASE(_terrains_descr_pool)
	SAFE_RELEASE(_vegetables_descr_pool)
}

void VulkanTerrainRenderer::Create(
	VulkanRenderPass* gbuffer_renderpass,
	VulkanDescriptorSet* transform_descr_set
) {

	//terrains descriptor pool
	_terrains_descr_pool = new VulkanDescriptorPool;
	_terrains_descr_pool->SetDescriptorSetsCount(1000);
	_terrains_descr_pool->SetCreateFreeDescrSets(true);
	_terrains_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000);
	_terrains_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6000);
	_terrains_descr_pool->Create();

	//vegetables descriptor pool
	_vegetables_descr_pool = new VulkanDescriptorPool;
	_vegetables_descr_pool->SetDescriptorSetsCount(1000);
	_vegetables_descr_pool->SetCreateFreeDescrSets(true);
	_vegetables_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000);
	_vegetables_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000);
	_vegetables_descr_pool->Create();

	if (!TerrainPipelinesInitialized)
	{
		InitTerrainPipelines(transform_descr_set, _vegetables_descr_pool, gbuffer_renderpass);
		TerrainPipelinesInitialized = true;
	}
}

void VulkanTerrainRenderer::ProcessTerrain(Entity* terrain) 
{
	TerrainComponent* terrain_component = terrain->GetComponent<TerrainComponent>();
	VulkanTerrain* vk_terrain = (VulkanTerrain*) terrain_component->GetTerrain();

	if (vk_terrain == nullptr) 
	{
		// Create new terrain if component doesnt have it
		vk_terrain = new VulkanTerrain;
		vk_terrain->Create(_terrains_descr_pool);
		vk_terrain->SetImagesToEmpty();
		terrain_component->SetTerrain(vk_terrain);
	}

	vk_terrain->SetTerrain(terrain_component);

	_terrains_processed++;
}

void VulkanTerrainRenderer::ResetProcessedTerrains() {
	_terrains_processed = 0;
	_vegetables_transforms_written = 0;
}

VulkanDescriptorPool* VulkanTerrainRenderer::GetTerrainDescriptorPool() {
	return _terrains_descr_pool;
}

VulkanDescriptorPool* VulkanTerrainRenderer::GetGrassDescriptorPool() {
	return _vegetables_descr_pool;
}

uint32& VulkanTerrainRenderer::GetWrittenGrassTransforms() {
	return _vegetables_transforms_written;
}

void VulkanTerrainRenderer::DrawTerrain(
	VulkanCommandBuffer* cmdbuffer,
	TerrainComponent* component,
	uint32 camera_index,
	VulkanDescriptorSet* entity_descr_set,
	uint32 draw_index,
	uint32 output_width,
	uint32 output_height,
	bool reverse_cull)
{
	VulkanTerrain* vk_terrain = (VulkanTerrain*) component->GetTerrain();

	uint32 offsets[2] = { camera_index * CAMERA_ELEM_SIZE, draw_index * UNI_ALIGN };

	VulkanMesh* terrain_mesh = (VulkanMesh*) component->GetTerrainMesh();
	if (terrain_mesh) 
	{
		cmdbuffer->BindPipeline(*terrain_pipeline);

		if (reverse_cull)
			cmdbuffer->SetCullMode(VK_CULL_MODE_BACK_BIT);
		else
			cmdbuffer->SetCullMode(VK_CULL_MODE_FRONT_BIT);

		cmdbuffer->SetViewport(0, 0, (float)output_width, (float)output_height);

		cmdbuffer->BindDescriptorSets(*terrain_pipeline_layout, 0, 1, entity_descr_set, 2, offsets);
		// info about terrain
		cmdbuffer->BindDescriptorSets(*terrain_pipeline_layout, 1, 1, vk_terrain->GetDescriptorSet(), 0, nullptr);

		cmdbuffer->BindMesh(*terrain_mesh);
		cmdbuffer->DrawIndexed(component->GetIndicesCount());
	}

	cmdbuffer->BindPipeline(*grass_pipeline);
	cmdbuffer->SetViewport(0, 0, (float)output_width, (float)output_height);
	cmdbuffer->BindDescriptorSets(*grass_pipeline_layout, 0, 1, entity_descr_set, 2, offsets);

	for (uint32 grass_i = 0; grass_i < vk_terrain->GetVegetables().size(); grass_i++) {
		auto& vegetable = vk_terrain->GetVegetables()[grass_i];
		uint32 offset = vegetable.offset * sizeof(Mat4);
		cmdbuffer->BindDescriptorSets(*grass_pipeline_layout, 1, 1, vegetable._descr_set, 1, &offset);
		cmdbuffer->BindMesh(*terrainGrassMesh);
		cmdbuffer->DrawIndexed(terrainGrassMesh->GetIndexCount(), vegetable.count);
	}
}