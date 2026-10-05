#include "VulkanRenderer.hpp"
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Scene/Entity.hpp>
#include "VulkanRenderingGenerics.hpp"
#include "../VulkanRAPI.hpp"

#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>

using namespace VSGE;

bool TerrainPipelinesInitialized = false;
static VulkanPipelineLayout* terrain_pipeline_layout = nullptr;
static VulkanPipeline* terrain_pipeline = nullptr;

void InitTerrainPipelines(VulkanDescriptorSet* vertexSet, VulkanDescriptorPool* pool, VulkanRenderPass* gbuffer_renderpass)
{
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

	vk_terrain->SetTerrain(terrain_component, _vegetables_descr_pool);

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

	for (uint32 grass_i = 0; grass_i < component->GetTerrainVegetables().size(); grass_i++)
	{
		auto& vegetable = component->GetTerrainVegetables()[grass_i];
		auto& vk_vegetable = vk_terrain->GetVegetables()[grass_i];

		// Extract material and mesh resource pointers
		MeshResource* mesh_resource = vegetable._mesh_reference.GetResource<MeshResource>();
		MaterialResource* mat_resource = vegetable._material_reference.GetResource<MaterialResource>();

		if (!mesh_resource || !mat_resource)
			continue;

		// Do not render if material or mesh not loaded yet
		if (mat_resource->GetState() != RESOURCE_STATE_READY || mesh_resource->GetState() != RESOURCE_STATE_READY)
			continue;

		// bind material
		MaterialTemplate* templ = mat_resource->GetMaterial()->GetTemplate();
		VulkanPipeline* pipeline = (VulkanPipeline*)templ->GetPipeline();
		Material* mat = mat_resource->GetMaterial();
		VulkanMaterial* vmat = (VulkanMaterial*)mat->GetDescriptors();
		VulkanMesh* mesh = (VulkanMesh*)mesh_resource->GetMesh();


		//Mark mesh resource used in this frame
		mesh_resource->Use();
		//Mark material resource used in this frame
		mat_resource->Use();

		uint32 offsets[2] = { camera_index * CAMERA_ELEM_SIZE, draw_index * UNI_ALIGN };
		uint32 veg_transform_offset = vk_vegetable.offset * sizeof(Mat4);

		if (templ->GetRenderStage() != RENDER_STAGE_GBUFFER)
			continue;

		cmdbuffer->BindPipeline(*pipeline);

		if (reverse_cull)
			cmdbuffer->SetCullMode(VK_CULL_MODE_BACK_BIT);
		else
			cmdbuffer->SetCullMode(VK_CULL_MODE_FRONT_BIT);

		cmdbuffer->SetViewport(0, 0, (float)output_width, (float)output_height);
		VulkanPipelineLayout* ppl = pipeline->GetPipelineLayout();
		cmdbuffer->BindDescriptorSets(*ppl, 1, 1, vmat->_fragmentDescriptorSet);
		cmdbuffer->BindDescriptorSets(*ppl, 0, 1, entity_descr_set, 2, offsets);
		cmdbuffer->BindDescriptorSets(*ppl, 2, 1, vk_vegetable._descr_set, 1, &veg_transform_offset);
		cmdbuffer->BindMesh(*mesh);
		if (mesh->GetIndexCount() > 0)
			cmdbuffer->DrawIndexed(mesh->GetIndexCount(), vk_vegetable.count);
		else
			cmdbuffer->Draw(mesh->GetVerticesCount(), vk_vegetable.count);
	}
}