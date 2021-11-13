#include "VulkanRenderer.hpp"
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Scene/Entity.hpp>

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
std::vector<VulkanTerrainGrass>& VulkanTerrain::GetVegetables() {
	return _grass_descriptor_sets;
}

void VulkanTerrain::SetDescriptorTexture(Resource* texture, uint32 texture_type, uint32 texture_index) {
	TextureResource* texture_res = static_cast<TextureResource*>(texture);
	if (texture_res == nullptr) {
		//if no texture bound, then bind default texture

		VulkanTexture* default_texture = VulkanRenderer::Get()->GetTerrainRenderer()->GetEmptyZeroTexture();
		if(texture_type == 2 || texture_type == 3 || texture_type == 4)
			default_texture = VulkanRenderer::Get()->GetTerrainRenderer()->GetEmptyWhiteTexture();

		_terrain_descr_set->WriteDescriptorImage(texture_type + 2,
			default_texture,
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

void VulkanTerrain::SetDescriptorGrassTexture(Resource* texture, uint32 vegetable_index) {
	TextureResource* texture_res = static_cast<TextureResource*>(texture);
	VulkanDescriptorSet* grass_set = _grass_descriptor_sets[vegetable_index]._descr_set;
	if (texture_res == nullptr) {
		//if no texture bound, then bind default texture
		VulkanTexture* default_texture = VulkanRenderer::Get()->GetTerrainRenderer()->GetEmptyZeroTexture();
		grass_set->WriteDescriptorImage(1,
			default_texture,
			VulkanRenderer::Get()->GetTerrainRenderer()->GetTerrainTextureSampler(),
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
			VulkanRenderer::Get()->GetTerrainRenderer()->GetTerrainTextureSampler(),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

void VulkanTerrain::SetTerrain(TerrainComponent* terrain, VulkanTerrainRenderer* terrain_renderer) {
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

	//Create vegetables descriptors
	uint32 grass_count = static_cast<uint32>(terrain->GetTerrainVegetables().size());
	while (_grass_descriptor_sets.size() < grass_count) {
		VulkanDescriptorSet* grass_descr_set = new VulkanDescriptorSet(terrain_renderer->GetGrassDescriptorPool());
		grass_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT);
		grass_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
		grass_descr_set->Create();
		grass_descr_set->WriteDescriptorBuffer(0, terrain_renderer->GetGrassTransformsBuffer());
		
		VulkanTerrainGrass grass;
		grass._descr_set = grass_descr_set;
		_grass_descriptor_sets.push_back(grass);
	}

	for (uint32 veg_i = 0; veg_i < grass_count; veg_i++) {
		auto& grass = terrain->GetTerrainVegetables()[veg_i];
		auto& grass_transforms = terrain->GetGrassTransforms()[veg_i];

		SetDescriptorGrassTexture(grass._texture_reference.GetResource(), veg_i);
		
		uint32& grass_offset = terrain_renderer->GetWrittenGrassTransforms();
		uint32 transforms_count = static_cast<uint32>(grass_transforms.GetTransforms().size());
		const Mat4* transforms = grass_transforms.GetTransforms().data();
		terrain_renderer->GetGrassTransformsBuffer()->WriteData(grass_offset * sizeof(Mat4), transforms_count * sizeof(Mat4), (void*)transforms);
		

		auto& grass_render_info = _grass_descriptor_sets[veg_i];
		grass_render_info.offset = grass_offset;
		grass_render_info.count = transforms_count;

		grass_offset += transforms_count;
	}
}
TerrainComponent* VulkanTerrain::GetTerrain() {
	return _terrain;
}
void VulkanTerrain::Create(VulkanDescriptorPool* pool, VulkanBuffer* terrains_buffer) {
	_terrain_descr_set = new VulkanDescriptorSet(pool);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 7, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES_PER_TERRAIN);
	_terrain_descr_set->Create();

	if(terrains_buffer)
		_terrain_descr_set->WriteDescriptorBuffer(0, terrains_buffer, 0, terrains_buffer->GetSize());
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
	_camera_index = 0;
	_reverse_cull = false;
}
VulkanTerrainRenderer::~VulkanTerrainRenderer() {
	SAFE_RELEASE(_terrain_pipeline)

	SAFE_RELEASE(_terrains_buffer)
	SAFE_RELEASE(_grass_transform_buffer)

	SAFE_RELEASE(_terrain_masks_sampler)
	SAFE_RELEASE(_terrain_textures_sampler)
}

void VulkanTerrainRenderer::Create(
	VulkanRenderPass* gbuffer_renderpass,
	std::vector<VulkanDescriptorSet*>& descr_set,
	VulkanTexture* emptyTexture,
	VulkanTexture* emptyOneTexture
) {
	this->_gbuffer_renderpass = gbuffer_renderpass;
	this->_entity_descr_set = &descr_set;
	this->_emptyZeroTexture = emptyTexture;
	_emptyOneTexture = emptyOneTexture;
	
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

	_terrain_shader = new VulkanShader;
	_terrain_shader->AddShaderFromFile("terrain.vert", SHADER_STAGE_VERTEX);
	_terrain_shader->AddShaderFromFile("terrain.frag", SHADER_STAGE_FRAGMENT);

	_grass_shader = new VulkanShader;
	_grass_shader->AddShaderFromFile("grass.vert", SHADER_STAGE_VERTEX);
	_grass_shader->AddShaderFromFile("grass.frag", SHADER_STAGE_FRAGMENT);

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

	VulkanDescriptorSet* grass_descr_set = new VulkanDescriptorSet(_vegetables_descr_pool);
	grass_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_VERTEX_BIT);
	grass_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	grass_descr_set->Create();
	_grass_pipeline_layout = new VulkanPipelineLayout;
	_grass_pipeline_layout->PushDescriptorSet(_entity_descr_set->at(0));
	_grass_pipeline_layout->PushDescriptorSet(grass_descr_set);
	_grass_pipeline_layout->Create();
	delete grass_descr_set;

	_grass_pipeline = new VulkanPipeline;
	_grass_pipeline->SetDepthTest(true);
	_grass_pipeline->Create(_grass_shader, gbuffer_renderpass, _vertexLayout, _grass_pipeline_layout);

	_terrain_pipeline = new VulkanPipeline;
	_terrain_pipeline->SetDepthTest(true);
	_terrain_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_terrain_pipeline->Create(_terrain_shader, gbuffer_renderpass, _vertexLayout, _terrain_pipeline_layout);

	_terrain_masks_sampler = new VulkanSampler;
	_terrain_masks_sampler->Create();

	_terrain_textures_sampler = new VulkanSampler;
	_terrain_textures_sampler->SetWrapModes(SAMPLER_WRAP_REPEAT, SAMPLER_WRAP_REPEAT);
	_terrain_textures_sampler->Create();

	_terrains_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_terrains_buffer->Create(65535);

	_grass_transform_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_STORAGE);
	_grass_transform_buffer->Create(MAX_GRASS_TRANSFORMS * sizeof(Mat4));


	CalculateNormals(grass_vertices, grass_ind, 24);
	ProcessTangentSpace(grass_vertices, grass_ind, 24);

	_grass_mesh = new VulkanMesh;
	_grass_mesh->SetVertexBuffer(grass_vertices, 8);
	_grass_mesh->SetIndexBuffer(grass_ind, 24);
	_grass_mesh->Create();
}

void VulkanTerrainRenderer::ProcessTerrain(Entity* terrain) {
	if (_terrains_processed + 1 > _terrains.size()) {
		VulkanTerrain* new_terrain = new VulkanTerrain;
		new_terrain->Create(_terrains_descr_pool, _terrains_buffer);
		new_terrain->SetImagesToEmpty();
		_terrains.push_back(new_terrain);
	}
	VulkanTerrain* vk_terrain = _terrains[_terrains_processed];
	TerrainComponent* terrain_component = terrain->GetComponent<TerrainComponent>();
	vk_terrain->SetTerrain(terrain_component, this);

	uint32 offset = _terrains_processed * TERRAIN_DATA_ELEM_SIZE;

	float uv_x = (float)terrain_component->GetWidth() / 64;
	float uv_y = (float)terrain_component->GetHeight() / 64;
	uint32 textures_count = (uint32)terrain_component->GetTerrainTextures().size();

	_terrains_buffer->WriteData(offset, 4, &uv_x);
	_terrains_buffer->WriteData(offset + 4, 4, &uv_y);
	_terrains_buffer->WriteData(offset + 8, 4, &textures_count);

	offset += 16;
	for (uint32 i = 0; i < terrain_component->GetTerrainTextures().size(); i++) {
		TerrainTexture* texture = &terrain_component->GetTerrainTextures()[i];
		_terrains_buffer->WriteData(offset, 4, &texture->_roughness_factor);
		_terrains_buffer->WriteData(offset + 4, 4, &texture->_metallic_factor);
		_terrains_buffer->WriteData(offset + 8, 4, &texture->_height_factor);
		offset += 16;
	}

	_terrains_processed++;
}

void VulkanTerrainRenderer::ResetProcessedTerrains() {
	_terrains_processed = 0;
	_vegetables_transforms_written = 0;
}

void VulkanTerrainRenderer::SetOutputSizes(uint32 width, uint32 height) {
	_outputWidth = width;
	_outputHeight = height;
}
void VulkanTerrainRenderer::SetCameraIndex(uint32 camera_index) {
	_camera_index = camera_index;
}
VulkanSampler* VulkanTerrainRenderer::GetTerrainMasksTextureSampler() {
	return _terrain_masks_sampler;
}

VulkanSampler* VulkanTerrainRenderer::GetTerrainTextureSampler() {
	return _terrain_textures_sampler;
}

VulkanTexture* VulkanTerrainRenderer::GetEmptyZeroTexture() {
	return _emptyZeroTexture;
}

VulkanTexture* VulkanTerrainRenderer::GetEmptyWhiteTexture() {
	return _emptyOneTexture;
}

VulkanDescriptorPool* VulkanTerrainRenderer::GetTerrainDescriptorPool() {
	return _terrains_descr_pool;
}

VulkanDescriptorPool* VulkanTerrainRenderer::GetGrassDescriptorPool() {
	return _vegetables_descr_pool;
}

VulkanBuffer* VulkanTerrainRenderer::GetGrassTransformsBuffer() {
	return _grass_transform_buffer;
}

VulkanBuffer* VulkanTerrainRenderer::GetTerrainsBuffer() {
	return _terrains_buffer;
}

uint32& VulkanTerrainRenderer::GetWrittenGrassTransforms() {
	return _vegetables_transforms_written;
}
void VulkanTerrainRenderer::SetReverseFaceCull(bool reverse) {
	_reverse_cull = reverse;
}
void VulkanTerrainRenderer::DrawTerrain(VulkanCommandBuffer* cmdbuffer, uint32 terrain_index, uint32 draw_index) {
	VulkanTerrain* vk_terrain = _terrains[terrain_index];
	TerrainComponent* terrain = vk_terrain->GetTerrain();

	uint32 offsets[2] = { _camera_index * CAMERA_ELEM_SIZE, draw_index * UNI_ALIGN % 65535 };
	int vertexDescriptorID = (draw_index * UNI_ALIGN) / 65535;

	VulkanMesh* terrain_mesh = (VulkanMesh*)terrain->GetTerrainMesh();
	if (terrain_mesh) {
		cmdbuffer->BindPipeline(*_terrain_pipeline);
		if (_reverse_cull)
			cmdbuffer->SetCullMode(VK_CULL_MODE_BACK_BIT);
		else
			cmdbuffer->SetCullMode(VK_CULL_MODE_FRONT_BIT);
		cmdbuffer->SetViewport(0, 0, (float)_outputWidth, (float)_outputHeight);
		uint32 terrain_data_offset = terrain_index * TERRAIN_DATA_ELEM_SIZE;

		cmdbuffer->BindDescriptorSets(*_terrain_pipeline_layout, 0, 1, _entity_descr_set->at(vertexDescriptorID), 2, offsets);
		cmdbuffer->BindDescriptorSets(*_terrain_pipeline_layout, 1, 1, vk_terrain->GetDescriptorSet(), 1, &terrain_data_offset);

		cmdbuffer->BindMesh(*terrain_mesh);
		cmdbuffer->DrawIndexed(terrain->GetIndicesCount());
	}

	cmdbuffer->BindPipeline(*_grass_pipeline);
	cmdbuffer->SetViewport(0, 0, (float)_outputWidth, (float)_outputHeight);
	cmdbuffer->BindDescriptorSets(*_grass_pipeline_layout, 0, 1, _entity_descr_set->at(vertexDescriptorID), 2, offsets);

	for (uint32 grass_i = 0; grass_i < vk_terrain->GetVegetables().size(); grass_i ++) {
		auto& vegetable = vk_terrain->GetVegetables()[grass_i];
		uint32 offset = vegetable.offset * sizeof(Mat4);
		cmdbuffer->BindDescriptorSets(*_grass_pipeline_layout, 1, 1, vegetable._descr_set, 1, &offset);
		cmdbuffer->BindMesh(*_grass_mesh);
		cmdbuffer->DrawIndexed(_grass_mesh->GetIndexCount(), vegetable.count);
	}
}