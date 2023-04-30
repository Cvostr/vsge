#include "VulkanShadowmapping.hpp"
#include "../VulkanRAPI.hpp"
#include "../VulkanPipeline.hpp"
#include <Resources/ResourceTypes/MeshResource.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/EntityComponents/MaterialComponent.hpp>
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Scene/Scene.hpp>
#include "VulkanRenderer.hpp"
#include "VulkanRenderingGenerics.hpp"

using namespace VSGE;

VulkanShadowmapping::VulkanShadowmapping(
	std::vector<VulkanDescriptorSet*>* vertexDescrSets,
	VulkanDescriptorSet* animsDescrSet,
	VulkanBuffer* cam_buffer,
	VulkanTexture* pos
){
	_bound_pipeline = nullptr;
	_writtenBones = 0;
	_writtenParticleTransforms = 0;
	_added_casters = 0;
	this->_vertexDescrSets = vertexDescrSets;
	this->animsDescrSet = animsDescrSet;
	_gbuffer_pos = pos;

	_outputWidth = 1280;
	_outputHeight = 720;

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();
	VulkanRenderer* renderer = VulkanRenderer::Get();

	_shadowmap_render_pass = new VulkanRenderPass;
	_shadowmap_render_pass->SetClearSize(MAP_SIZE, MAP_SIZE);
	_shadowmap_render_pass->PushDepthAttachment(FORMAT_DEPTH_32);
	_shadowmap_render_pass->Create();

	shadowmap_vertex_layout.AddBinding(sizeof(Vertex));
	shadowmap_vertex_layout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);

	shadowmap_vertex_layout.AddBinding(sizeof(VertexSkinningData));
	shadowmap_vertex_layout.AddItem(5, offsetof(VertexSkinningData, ids), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);
	shadowmap_vertex_layout.AddItem(6, offsetof(VertexSkinningData, ids[4]), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);
	shadowmap_vertex_layout.AddItem(7, offsetof(VertexSkinningData, ids[8]), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);
	shadowmap_vertex_layout.AddItem(8, offsetof(VertexSkinningData, weights), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);
	shadowmap_vertex_layout.AddItem(9, offsetof(VertexSkinningData, weights[4]), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);
	shadowmap_vertex_layout.AddItem(10, offsetof(VertexSkinningData, weights[8]), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);
	shadowmap_vertex_layout.AddItem(11, offsetof(VertexSkinningData, bones_num), VertexLayoutFormat::VL_FORMAT_R32_UINT, 1);

	shadowmap_terrain_vertex_layout.AddBinding(sizeof(Vertex));
	shadowmap_terrain_vertex_layout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);

	_shadowcasters_buffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
	_shadowcasters_buffer->Create(65536);

	_shadowprocess_buffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
	_shadowprocess_buffer->Create(SHADOWPROCESS_BUFFER_SIZE);

	_cascadeinfo_buffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
	_cascadeinfo_buffer->Create(168);

	_shadowcasters_descPool = new VulkanDescriptorPool;
	_shadowcasters_descPool->SetDescriptorSetsCount(2);
	_shadowcasters_descPool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 50);
	_shadowcasters_descPool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50);
	_shadowcasters_descPool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100);

	_shadowcaster_descrSet = new VulkanDescriptorSet(_shadowcasters_descPool);
	_shadowcaster_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 0, VK_SHADER_STAGE_ALL_GRAPHICS);
	_shadowcaster_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL_GRAPHICS);

	_shadowrenderer_descrSet = new VulkanDescriptorSet(_shadowcasters_descPool);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, VK_SHADER_STAGE_FRAGMENT_BIT);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT, 2);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT, 32);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, VK_SHADER_STAGE_FRAGMENT_BIT, 32);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 6, VK_SHADER_STAGE_FRAGMENT_BIT);

	_shadowcasters_descPool->Create();

	_shadowcaster_descrSet->Create();
	_shadowcaster_descrSet->WriteDescriptorBuffer(0, _shadowcasters_buffer);
	_shadowcaster_descrSet->WriteDescriptorBuffer(1, _cascadeinfo_buffer);

	_shadowrenderer_descrSet->Create();
	_shadowrenderer_descrSet->WriteDescriptorBuffer(0, _shadowprocess_buffer);
	_shadowrenderer_descrSet->WriteDescriptorImage(1, _gbuffer_pos, renderer->GetAttachmentSampler());
	_shadowrenderer_descrSet->WriteDescriptorBuffer(2, cam_buffer);
	_shadowrenderer_descrSet->WriteDescriptorBuffer(6, _cascadeinfo_buffer);

	
	_shadowmap_shader = new VulkanShader;
	_shadowmap_shader->AddShaderFromFile("shadowmap.vert", SHADER_STAGE_VERTEX);
	_shadowmap_shader->AddShaderFromFile("shadowmap.geom", SHADER_STAGE_GEOMETRY);

	_shadowmap_terrain_shader = new VulkanShader;
	_shadowmap_terrain_shader->AddShaderFromFile("shadowmap_terrain.vert", SHADER_STAGE_VERTEX);
	_shadowmap_terrain_shader->AddShaderFromFile("shadowmap.geom", SHADER_STAGE_GEOMETRY);

	_shadowmap_point_shader = new VulkanShader;
	_shadowmap_point_shader->AddShaderFromFile("shadowmap.vert", SHADER_STAGE_VERTEX);
	_shadowmap_point_shader->AddShaderFromFile("shadowmap.frag", SHADER_STAGE_FRAGMENT);
	_shadowmap_point_shader->AddShaderFromFile("shadowmap.geom", SHADER_STAGE_GEOMETRY);

	_shadowmap_terrain_point_shader = new VulkanShader;
	_shadowmap_terrain_point_shader->AddShaderFromFile("shadowmap_terrain.vert", SHADER_STAGE_VERTEX);
	_shadowmap_terrain_point_shader->AddShaderFromFile("shadowmap.frag", SHADER_STAGE_FRAGMENT);
	_shadowmap_terrain_point_shader->AddShaderFromFile("shadowmap.geom", SHADER_STAGE_GEOMETRY);

	_shadowmap_layout = new VulkanPipelineLayout;
	_shadowmap_layout->PushDescriptorSet(vertexDescrSets->at(0));
	_shadowmap_layout->PushDescriptorSet(_shadowcaster_descrSet);
	_shadowmap_layout->PushDescriptorSet(animsDescrSet);
	_shadowmap_layout->AddPushConstantRange(0, 8, VK_SHADER_STAGE_GEOMETRY_BIT);
	_shadowmap_layout->Create();

	_shadowmap_pipeline = new VulkanPipeline;
	_shadowmap_pipeline->SetDepthTest(true);
	_shadowmap_pipeline->SetDepthClamp(true);
	_shadowmap_pipeline->SetCompareOp(CompareOp::COMPARE_OP_LESS_OR_EQUAL);
	_shadowmap_pipeline->SetDynamicCullMode(false);
	_shadowmap_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_shadowmap_pipeline->Create(
		_shadowmap_shader, 
		_shadowmap_render_pass,
		shadowmap_vertex_layout,
		_shadowmap_layout);

	_shadowmap_point_pipeline = new VulkanPipeline;
	_shadowmap_point_pipeline->SetDepthTest(true);
	_shadowmap_point_pipeline->SetDynamicCullMode(false);
	_shadowmap_point_pipeline->SetCullMode(CullMode::CULL_MODE_BACK);
	_shadowmap_point_pipeline->Create(
		_shadowmap_point_shader,
		_shadowmap_render_pass,
		shadowmap_vertex_layout,
		_shadowmap_layout);

	_shadowmap_terrain_pipeline = new VulkanPipeline;
	_shadowmap_terrain_pipeline->SetDepthTest(true);
	_shadowmap_terrain_pipeline->SetDepthClamp(true);
	_shadowmap_terrain_pipeline->SetCompareOp(CompareOp::COMPARE_OP_LESS_OR_EQUAL);
	_shadowmap_terrain_pipeline->SetDynamicCullMode(false);
	_shadowmap_terrain_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_shadowmap_terrain_pipeline->Create(
		_shadowmap_terrain_shader,
		_shadowmap_render_pass,
		shadowmap_terrain_vertex_layout,
		_shadowmap_layout);

	_shadowmap_terrain_point_pipeline = new VulkanPipeline;
	_shadowmap_terrain_point_pipeline->SetDepthTest(true);
	_shadowmap_terrain_point_pipeline->SetDynamicCullMode(false);
	_shadowmap_terrain_point_pipeline->SetCullMode(CullMode::CULL_MODE_BACK);
	_shadowmap_terrain_point_pipeline->Create(
		_shadowmap_terrain_point_shader,
		_shadowmap_render_pass,
		shadowmap_terrain_vertex_layout,
		_shadowmap_layout);

	//-------------------------SHADOW PROCESS -----------------------------
	_shadowprocessRenderPass = new VulkanRenderPass;
	_shadowprocessRenderPass->SetClearSize(_outputWidth, _outputHeight);
	_shadowprocessRenderPass->PushColorAttachment(FORMAT_RGBA);
	_shadowprocessRenderPass->Create();

	_shadowprocess_framebuffer = new VulkanFramebuffer;
	_shadowprocess_framebuffer->SetSize(_outputWidth, _outputHeight);
	_shadowprocess_framebuffer->AddAttachment();
	_shadowprocess_framebuffer->Create(_shadowprocessRenderPass);

	_shadowprocess_shader = new VulkanShader;
	_shadowprocess_shader->AddShaderFromFile("shadowmap_process.vert", SHADER_STAGE_VERTEX);
	_shadowprocess_shader->AddShaderFromFile("shadowmap_process.frag", SHADER_STAGE_FRAGMENT);
	
	_shadowprocess_vertex_layout.AddBinding(sizeof(Vertex));
	_shadowprocess_vertex_layout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_shadowprocess_vertex_layout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	_shadowprocess_layout = new VulkanPipelineLayout;
	_shadowprocess_layout->PushDescriptorSet(_shadowrenderer_descrSet);
	_shadowprocess_layout->Create();

	_shadowprocess_pipeline = new VulkanPipeline;
	_shadowprocess_pipeline->SetDynamicCullMode(false);
	_shadowprocess_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_shadowprocess_pipeline->Create(_shadowprocess_shader, _shadowprocessRenderPass, _shadowprocess_vertex_layout, _shadowprocess_layout);

	_shadowmap_sampler = new VulkanSampler;
	_shadowmap_sampler->SetFilteringModes(SAMPLER_FILTERING_NEAREST, SAMPLER_FILTERING_NEAREST);
	_shadowmap_sampler->SetWrapModes(SAMPLER_WRAP_CLAMP_TO_BORDER, SAMPLER_WRAP_CLAMP_TO_BORDER, SAMPLER_WRAP_CLAMP_TO_BORDER);
	_shadowmap_sampler->SetBorderColor(BORDER_COLOR_OPAQUE_WHITE);
	_shadowmap_sampler->Create();

	//write empty textures to shadowmaps
	for (uint32 i = 0; i < 32; i++) {
		if (i < 2) {
			_shadowrenderer_descrSet->WriteDescriptorImage(3, VulkanRenderingGenerics::Get()->GetBlack2dArrayTexture(), _shadowmap_sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i);
		}
		_shadowrenderer_descrSet->WriteDescriptorImage(4, VulkanRenderingGenerics::Get()->GetBlackCubeTexture(), _shadowmap_sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i);
		_shadowrenderer_descrSet->WriteDescriptorImage(5, VulkanRenderingGenerics::Get()->GetBlackTexture(), _shadowmap_sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i);
	}
}
VulkanShadowmapping::~VulkanShadowmapping() {
	SAFE_RELEASE(_shadowcasters_buffer);
	SAFE_RELEASE(_shadowprocess_buffer);
	SAFE_RELEASE(_cascadeinfo_buffer);

	//Destroy renderpasses
	SAFE_RELEASE(_shadowmap_render_pass);
	SAFE_RELEASE(_shadowprocessRenderPass);

	SAFE_RELEASE(_shadowprocess_framebuffer)
	//Release all casters
	for (auto& caster : _casters) {
		SAFE_RELEASE(caster);
	}
	_casters.clear();
}

uint32 VulkanShadowmapping::GetShadowTextureIndex(uint32 caster_index, uint32 caster_type) {
	uint32 pcaster_index = 0;
	for (uint32 i = 0; i < caster_index; i++) {
		if (_casters[i]->_lightsource->GetLightType() == caster_type)
			pcaster_index++;
	}
	return pcaster_index;
}

void VulkanShadowmapping::SetCamera(Camera* cam) {
	this->cam = cam;
}
void VulkanShadowmapping::SetEntitiesToRender(tEntityList* entities) {
	_entitiesToRender = entities;
}
void VulkanShadowmapping::SetTerrainsToRender(tEntityList* terrains) {
	_terrainsToRender = terrains;
}

void VulkanShadowmapping::AddEntity(Entity* entity) {
	Scene* scene = VulkanRenderer::Get()->GetScene();
	LightsourceComponent* light = entity->GetComponent<LightsourceComponent>();
	LightType caster_type = light->GetLightType();

	VulkanShadowCaster* caster = nullptr;

	if (_added_casters == _casters.size()) {
		caster = new VulkanShadowCaster;
		_casters.push_back(caster);
	}
	else {
		caster = _casters[_added_casters];
	}

	uint32 image_layers_count = 6;
	if (caster_type == LIGHT_TYPE_DIRECTIONAL) {
		SceneEnvironmentSettings& env_settings = scene->GetEnvironmentSettings();
		image_layers_count = env_settings.GetShadowCascadesCount();
	}
	else if (caster_type == LIGHT_TYPE_SPOT)
		image_layers_count = 1;

	uint32 layers = caster->_framebuffer->GetLayersCount();
	if (layers != image_layers_count || caster->_lightsource->GetLightType() != caster_type) {
		bool is_point = (caster_type == LIGHT_TYPE_POINT);
		bool is_spot = (caster_type == LIGHT_TYPE_SPOT);
		caster->_framebuffer->Destroy();
		caster->_framebuffer->SetSize(MAP_SIZE, MAP_SIZE);
		caster->_framebuffer->SetLayersCount(image_layers_count);
		if (is_point) {
			caster->_framebuffer->AddDepth(FORMAT_DEPTH_32, image_layers_count, true);
			caster->_framebuffer->Create(_shadowmap_render_pass);
		}
		else {
			caster->_framebuffer->AddDepth(FORMAT_DEPTH_32, image_layers_count);
			caster->_framebuffer->Create(_shadowmap_render_pass);
		}
	}

	caster->_lightsource = light;

	Mat4* cascades_projections = light->GetShadowcastMatrices(cam);
	_shadowcasters_buffer->WriteData(_added_casters * SHADOWMAP_BUFFER_ELEMENT_SIZE, sizeof(Mat4) * image_layers_count, cascades_projections);
	_shadowprocess_buffer->WriteData(_added_casters * SHADOWPROCESS_BUFFER_ELEMENT_SIZE, sizeof(Mat4) * image_layers_count, cascades_projections);
	SAFE_RELEASE_ARR(cascades_projections);

	uint32 offset_casters = _added_casters * SHADOWMAP_BUFFER_ELEMENT_SIZE + sizeof(Mat4) * 10;
	uint32 offset = _added_casters * SHADOWPROCESS_BUFFER_ELEMENT_SIZE + sizeof(Mat4) * 10;

	float bias = light->GetShadowsBias();
	int size = MAP_SIZE;
	uint32 pcf = light->GetShadowPCF();
	float strength = light->GetShadowStrength();
	LightType type = light->GetLightType();
	Vec3 pos = light->GetEntity()->GetAbsolutePosition();
	float range = light->GetRange();

	_shadowprocess_buffer->WriteData(offset, 4, &bias);
	_shadowprocess_buffer->WriteData(offset + 4, 4, &pcf);
	_shadowprocess_buffer->WriteData(offset + 8, 4, &strength);
	_shadowprocess_buffer->WriteData(offset + 12, 4, &type);
	_shadowprocess_buffer->WriteData(offset + 16, 12, &pos);
	_shadowprocess_buffer->WriteData(offset + 28, 4, &range);
	uint32 index = GetShadowTextureIndex(_added_casters, type);
	_shadowprocess_buffer->WriteData(offset + 32, 4, &index);

	_shadowcasters_buffer->WriteData(offset_casters, 12, &pos);
	_shadowcasters_buffer->WriteData(offset_casters + 12, 4, &type);
	_shadowcasters_buffer->WriteData(offset_casters + 16, 4, &range);

	_added_casters++;
}

void VulkanShadowmapping::ResizeOutput(uint32 width, uint32 height) {
	_outputWidth = width;
	_outputHeight = height;
	_shadowprocessRenderPass->SetClearSize(width, height);
	_shadowprocess_framebuffer->Resize(width, height);
}

void VulkanShadowmapping::SetGbufferPositionsAttachment(VulkanTexture* gpos) {
	_shadowrenderer_descrSet->WriteDescriptorImage(1, gpos,
		VulkanRenderer::Get()->GetAttachmentSampler());
}

VulkanTexture* VulkanShadowmapping::GetOutputTexture() {
	return (VulkanTexture*)_shadowprocess_framebuffer->GetColorAttachments()[0];
}

void VulkanShadowmapping::ResetCasters() {
	_added_casters = 0;
}

void VulkanShadowmapping::ProcessShadowCasters(VulkanCommandBuffer* cmdbuffer) {
	for (uint32 caster_i = 0; caster_i < _added_casters; caster_i++) {
		ProcessShadowCaster(caster_i, cmdbuffer);
	}
}

void VulkanShadowmapping::BindPipeline(VulkanCommandBuffer* cmdbuffer, LightType light_type, int mesh_type, uint32 dyn_offset) {
	VulkanPipeline* pipeline = _shadowmap_pipeline;
	if (mesh_type == 1)
		pipeline = _shadowmap_terrain_pipeline;

	if (light_type == LIGHT_TYPE_POINT) {
		pipeline = _shadowmap_point_pipeline;
		if (mesh_type == 1)
			pipeline = _shadowmap_terrain_point_pipeline;
	}
	
	if (_bound_pipeline != pipeline) {
		cmdbuffer->BindPipeline(*pipeline);
		cmdbuffer->SetViewport(0, 0, MAP_SIZE, MAP_SIZE);
		cmdbuffer->BindDescriptorSets(*_shadowmap_layout, 1, 1, _shadowcaster_descrSet, 1, &dyn_offset);
		_bound_pipeline = pipeline;
	}
}

void VulkanShadowmapping::ProcessShadowCaster(uint32 casterIndex, VulkanCommandBuffer* cmdbuf) {
	Scene* scene = VulkanRenderer::Get()->GetScene();
	VulkanShadowCaster* caster = _casters[casterIndex];
	uint32 cascades = caster->_framebuffer->GetLayersCount();
	LightType light_type = caster->_lightsource->GetLightType();

	VulkanFramebuffer* fb = caster->_framebuffer;

	uint32 vertexOffsets[] = { 0, 0 };
	uint32 shadowmap_offset = casterIndex * SHADOWMAP_BUFFER_ELEMENT_SIZE;

	_writtenBones = 0;
	_writtenParticleTransforms = 0;

	Frustum* _dir_caster_frustums = nullptr;
	if (caster->_lightsource->GetLightType() == LIGHT_TYPE_DIRECTIONAL) {
		SceneEnvironmentSettings& env_settings = scene->GetEnvironmentSettings();
		_dir_caster_frustums = new Frustum[env_settings.GetShadowCascadesCount()];
		Mat4* cascades_projections = caster->_lightsource->GetShadowcastMatrices(cam);
		for (uint32 c_i = 0; c_i < env_settings.GetShadowCascadesCount(); c_i++) {
			_dir_caster_frustums[c_i].Update(cascades_projections[c_i]);
		}
		SAFE_RELEASE_ARR(cascades_projections)
	}
	
	_bound_pipeline = nullptr;
	_shadowmap_render_pass->CmdBegin(*cmdbuf, *fb);

	for (uint32 e_i = 0; e_i < _entitiesToRender->size(); e_i++) {
		Entity* entity = _entitiesToRender->at(e_i);
		
		MaterialComponent* material_comp = entity->GetComponent<MaterialComponent>();
		if (material_comp) {
			if (!material_comp->IsCastShadows())
				continue;
		}

		
		VulkanMesh* mesh = nullptr;

		MeshComponent* mesh_component = entity->GetComponent<MeshComponent>();
		if (mesh_component) {
			MeshResource* mesh_resource = mesh_component->GetMeshResource();
			if (!mesh_resource)
				continue;

			if (mesh_resource->GetState() == RESOURCE_STATE_READY) {
				//Mark mesh resource used in this frame
				mesh_resource->Use();
				mesh = (VulkanMesh*)mesh_resource->GetMesh();
				BindPipeline(cmdbuf, light_type, 0, shadowmap_offset);
			}
		}
		else {
			TerrainComponent* terrain = entity->GetComponent<TerrainComponent>();
			if (terrain->IsShadowCastEnabled()) {
				mesh = (VulkanMesh*)terrain->GetTerrainMesh();
				BindPipeline(cmdbuf, light_type, 1, shadowmap_offset);
			}
		}

		if (caster->_lightsource->GetLightType() == LIGHT_TYPE_DIRECTIONAL) {
			AABB bbox = entity->GetAABB();
			SceneEnvironmentSettings& env_settings = scene->GetEnvironmentSettings();
			int first = -1;
			int last = -1;
			for (uint32 i = 0; i < env_settings.GetShadowCascadesCount(); i++) {
				if (_dir_caster_frustums[i].CheckAABB(bbox) != FRUSTUM_OUTSIDE) {
					if (first < 0) {
						first = i;
						last = i;
					}
				}
				if (first >= 0)
					last++;
			}

			if (first < 0)
				continue;

			int32 pconstants[2];
			pconstants[0] = first;
			pconstants[1] = last - 1;
			cmdbuf->PushConstants(*_shadowmap_layout, VK_SHADER_STAGE_GEOMETRY_BIT, 0, 8, pconstants);
		}

		//Check distance
		if (caster->_lightsource->GetLightType() == LIGHT_TYPE_POINT || caster->_lightsource->GetLightType() == LIGHT_TYPE_SPOT) {
			float distance = caster->_lightsource->GetEntity()->GetPosition().DistanceTo(entity->GetAbsolutePosition());
			if (distance > caster->_lightsource->GetRange() * 3)
				continue;
		}

		if (mesh) {
			uint32 offsets[2] = { 0, e_i * UNI_ALIGN };
			uint32 anim_offset = _writtenBones * 64;
			_writtenBones += (uint32)mesh->GetBones().size();

			int vertexDescriptorID = (e_i * UNI_ALIGN) / 65535;

			cmdbuf->BindDescriptorSets(*_shadowmap_layout, 0, 1, _vertexDescrSets->at(vertexDescriptorID), 2, offsets);
			cmdbuf->BindDescriptorSets(*_shadowmap_layout, 2, 1, animsDescrSet, 1, &anim_offset);
			cmdbuf->BindMesh(*mesh);
			if (mesh->GetIndexCount() > 0)
				cmdbuf->DrawIndexed(mesh->GetIndexCount());
			else
				cmdbuf->Draw(mesh->GetVerticesCount());
		}
	}

	SAFE_RELEASE_ARR(_dir_caster_frustums)

	cmdbuf->EndRenderPass();
}

void VulkanShadowmapping::RecordShadowProcessingCmdbuf(VulkanCommandBuffer* cmdbuf) {
	_shadowprocessRenderPass->CmdBegin(*cmdbuf, *_shadowprocess_framebuffer);
	cmdbuf->BindPipeline(*_shadowprocess_pipeline);
	cmdbuf->SetViewport(0, 0, _outputWidth, _outputHeight);
	cmdbuf->BindDescriptorSets(*_shadowprocess_layout, 0, 1, _shadowrenderer_descrSet);
	VulkanMesh* screenPlane = VulkanRenderer::Get()->GetScreenMesh();
	cmdbuf->BindMesh(*screenPlane);
	cmdbuf->DrawIndexed(6);
	cmdbuf->EndRenderPass();
}

void VulkanShadowmapping::UpdateShadowrenderingDescriptors() {
	//Write shadowmaps to descriptor set
	std::vector<VulkanTexture*> shadowmaps;
	std::vector<VulkanTexture*> shadowmaps_point;
	std::vector<VulkanTexture*> shadowmaps_spot;
	for (auto caster : _casters) {
		if (caster->_lightsource->GetLightType() == LIGHT_TYPE_POINT)
			shadowmaps_point.push_back((VulkanTexture*)caster->_framebuffer->GetDepthAttachment());
		else if (caster->_lightsource->GetLightType() == LIGHT_TYPE_SPOT)
			shadowmaps_spot.push_back((VulkanTexture*)caster->_framebuffer->GetDepthAttachment());

		else
			shadowmaps.push_back((VulkanTexture*)caster->_framebuffer->GetDepthAttachment());
	}

	_shadowrenderer_descrSet->WriteDescriptorImages(3,
		shadowmaps.data(),
		_shadowmap_sampler,
		static_cast<uint32>(shadowmaps.size())
	);
	_shadowrenderer_descrSet->WriteDescriptorImages(4,
		shadowmaps_point.data(),
		_shadowmap_sampler,
		static_cast<uint32>(shadowmaps_point.size())
	);
	_shadowrenderer_descrSet->WriteDescriptorImages(5,
		shadowmaps_spot.data(),
		_shadowmap_sampler,
		static_cast<uint32>(shadowmaps_spot.size())
	);
	_shadowprocess_buffer->WriteData(SHADOWPROCESS_SHADOWCOUNT_OFFSET, 4, &_added_casters);

	Scene* scene = VulkanRenderer::Get()->GetScene();
	if (scene) {
		SceneEnvironmentSettings& env_settings = scene->GetEnvironmentSettings();
		uint32 cascades_count = env_settings.GetShadowCascadesCount();
		for (uint32 cascade = 0; cascade < cascades_count; cascade++) {
			float cascade_depth = env_settings.GetCascadeDepths()[cascade];
			_cascadeinfo_buffer->WriteData(16 * cascade, 4, &cascade_depth);
		}
		_cascadeinfo_buffer->WriteData(160, 4, &cascades_count);
		int map_size = MAP_SIZE;
		_cascadeinfo_buffer->WriteData(164, 4, &map_size);
	}
}