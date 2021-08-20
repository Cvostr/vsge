#include "VulkanShadowmapping.hpp"
#include "../VulkanRAPI.hpp"
#include "../VulkanPipeline.hpp"
#include <Resources/ResourceTypes/MeshResource.hpp>
#include <Scene/EntityComponents/MeshComponent.hpp>
#include <Scene/Scene.hpp>

using namespace VSGE;

VulkanShadowmapping::VulkanShadowmapping(
	std::vector<VulkanDescriptorSet*>* vertexDescrSets,
	VulkanDescriptorSet* animsDescrSet,
	VulkanBuffer* cam_buffer,
	VulkanMesh* screenPlane,
	VulkanTexture* pos,
	VulkanSampler* gbuffer_sampler,
	VulkanTexture* empty_texture)
{
	_added_casters = 0;
	this->_vertexDescrSets = vertexDescrSets;
	this->animsDescrSet = animsDescrSet;
	_gbuffer_pos = pos;
	_gbuffer_sampler = gbuffer_sampler;
	_screenPlane = screenPlane;
	_empty_texture = empty_texture;

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_shadowmapFirstSemaphore = new VulkanSemaphore;
	_shadowmapFirstSemaphore->Create();

	_shadowmapSecondSemaphore = new VulkanSemaphore;
	_shadowmapSecondSemaphore->Create();

	_shadowmapRenderPass = new VulkanRenderPass;
	_shadowmapRenderPass->SetClearSize(2048, 2048);
	_shadowmapRenderPass->PushDepthAttachment(FORMAT_DEPTH_32);
	_shadowmapRenderPass->Create();

	_shadowmap_point_RenderPass = new VulkanRenderPass;
	_shadowmap_point_RenderPass->SetClearSize(2048, 2048);
	_shadowmap_point_RenderPass->PushColorAttachment(FORMAT_R32F);
	_shadowmap_point_RenderPass->Create();

	_shadowmapCmdPool = new VulkanCommandPool;
	_shadowmapCmdPool->Create(device->GetGraphicsQueueFamilyIndex());

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


	_shadowcasters_buffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
	_shadowcasters_buffer->Create(65536);

	_shadowprocess_buffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_UNIFORM);
	_shadowprocess_buffer->Create(43012);

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
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT, 33);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT, 32);
	_shadowrenderer_descrSet->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 5, VK_SHADER_STAGE_FRAGMENT_BIT);

	_shadowcasters_descPool->Create();

	_shadowcaster_descrSet->Create();
	_shadowcaster_descrSet->WriteDescriptorBuffer(0, _shadowcasters_buffer, 0, 65535);
	_shadowcaster_descrSet->WriteDescriptorBuffer(1, _cascadeinfo_buffer);

	_shadowrenderer_descrSet->Create();
	_shadowrenderer_descrSet->WriteDescriptorBuffer(0, _shadowprocess_buffer);
	_shadowrenderer_descrSet->WriteDescriptorImage(1, _gbuffer_pos, _gbuffer_sampler);
	_shadowrenderer_descrSet->WriteDescriptorBuffer(2, cam_buffer);
	_shadowrenderer_descrSet->WriteDescriptorBuffer(5, _cascadeinfo_buffer);

	


	_shadowmap_shader = new VulkanShader;
	_shadowmap_shader->AddShaderFromFile("shadowmap.vert", SHADER_STAGE_VERTEX);
	_shadowmap_shader->AddShaderFromFile("shadowmap.geom", SHADER_STAGE_GEOMETRY);

	_shadowmap_point_shader = new VulkanShader;
	_shadowmap_point_shader->AddShaderFromFile("shadowmap.vert", SHADER_STAGE_VERTEX);
	_shadowmap_point_shader->AddShaderFromFile("shadowmap.frag", SHADER_STAGE_FRAGMENT);
	_shadowmap_point_shader->AddShaderFromFile("shadowmap.geom", SHADER_STAGE_GEOMETRY);

	_shadowmap_layout = new VulkanPipelineLayout;
	_shadowmap_layout->PushDescriptorSet(vertexDescrSets->at(0));
	_shadowmap_layout->PushDescriptorSet(_shadowcaster_descrSet);
	_shadowmap_layout->PushDescriptorSet(animsDescrSet);
	_shadowmap_layout->Create();

	_shadowmapPipeline = new VulkanPipeline;
	_shadowmapPipeline->SetDepthTest(true);
	_shadowmapPipeline->SetDepthClamp(true);
	_shadowmapPipeline->SetCompareOp(CompareOp::COMPARE_OP_LESS_OR_EQUAL);
	_shadowmapPipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_shadowmapPipeline->Create(_shadowmap_shader, _shadowmapRenderPass, shadowmap_vertex_layout, _shadowmap_layout);

	_shadowmap_point_Pipeline = new VulkanPipeline;
	_shadowmap_point_Pipeline->SetDepthTest(false);
	_shadowmap_point_Pipeline->SetCullMode(CullMode::CULL_MODE_BACK);
	_shadowmap_point_Pipeline->Create(
		_shadowmap_point_shader,
		_shadowmap_point_RenderPass,
		shadowmap_vertex_layout,
		_shadowmap_layout);

	//-------------------------SHADOW PROCESS -----------------------------
	_shadowprocessRenderPass = new VulkanRenderPass;
	_shadowprocessRenderPass->SetClearSize(1280, 720);
	_shadowprocessRenderPass->PushColorAttachment(FORMAT_RGBA);
	_shadowprocessRenderPass->Create();

	_shadowprocess_framebuffer = new VulkanFramebuffer;
	_shadowprocess_framebuffer->SetSize(1280, 720);
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
	_shadowprocess_pipeline->Create(_shadowprocess_shader, _shadowprocessRenderPass, _shadowprocess_vertex_layout, _shadowprocess_layout);

	_shadowmap_sampler = new VulkanSampler;
	_shadowmap_sampler->SetFilteringModes(SAMPLER_FILTERING_NEAREST, SAMPLER_FILTERING_NEAREST);
	_shadowmap_sampler->SetWrapModes(SAMPLER_WRAP_CLAMP_TO_BORDER, SAMPLER_WRAP_CLAMP_TO_BORDER, SAMPLER_WRAP_CLAMP_TO_BORDER);
	_shadowmap_sampler->SetBorderColor(BORDER_COLOR_OPAQUE_WHITE);
	_shadowmap_sampler->Create();

	//--------------SHADOW PROCESS CMDBUF
	_shadowprocess_cmdbuf = new VulkanCommandBuffer();
	_shadowprocess_cmdbuf->Create(_shadowmapCmdPool);
	
	RecordShadowProcessingCmdbuf();

}
VulkanShadowmapping::~VulkanShadowmapping() {
	//delete _shadowmapFirstSemaphore;
	//delete _shadowmapSecondSemaphore;

	/*delete _shadowmapRenderPass;

	delete _shadowmapCmdPool;

	delete _shadowmapPipeline;
	delete _shadowmap_layout;
	delete _shadowmap_shader;

	delete _shadowprocess_pipeline;
	delete _shadowprocess_layout;
	delete _shadowprocess_shader;
	
	delete _shadowcasters_buffer;
	delete _shadowprocess_buffer;*/
}

void VulkanShadowmapping::AddEntity(Entity* entity) {
	LightsourceComponent* light = entity->GetComponent<LightsourceComponent>();
	LightType caster_type = light->GetLightType();

	VulkanShadowCaster* caster = nullptr;

	if (_added_casters == _casters.size()) {
		caster = new VulkanShadowCaster;
		_casters.push_back(caster);

		caster->_cmdbuf->Create(_shadowmapCmdPool);
	}
	else {
		caster = _casters[_added_casters];
	}

	uint32 image_layers_count = 6;
	if(caster_type == LIGHT_TYPE_DIRECTIONAL){
		SceneEnvironmentSettings& env_settings = _scene->GetEnvironmentSettings();
		image_layers_count = env_settings.GetShadowCascadesCount();
	}

	uint32 layers = caster->_framebuffer->GetLayersCount();
	if (layers != image_layers_count || caster->_caster_type != caster_type) {
		bool is_point = (caster_type == LIGHT_TYPE_POINT);
		caster->_framebuffer->Destroy();
		caster->_framebuffer->SetSize(MAP_SIZE, MAP_SIZE);
		caster->_framebuffer->SetLayersCount(image_layers_count);
		if (is_point) {
			caster->_framebuffer->AddAttachment(FORMAT_R32F, 6, true);
			caster->_framebuffer->Create(_shadowmap_point_RenderPass);
		}
		else {
			caster->_framebuffer->AddDepth(FORMAT_DEPTH_32, image_layers_count);
			caster->_framebuffer->Create(_shadowmapRenderPass);
		}
	}

	caster->_caster_type = entity->GetComponent<LightsourceComponent>()->GetLightType();

	Mat4* cascades_projections = light->GetShadowcastMatrices(cam);
	_shadowcasters_buffer->WriteData(_added_casters * SHADOWMAP_BUFFER_ELEMENT_SIZE, sizeof(Mat4) * image_layers_count, cascades_projections);
	_shadowprocess_buffer->WriteData(_added_casters * SHADOWPROCESS_BUFFER_ELEMENT_SIZE, sizeof(Mat4) * image_layers_count, cascades_projections);
	delete[] cascades_projections;

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

	_shadowcasters_buffer->WriteData(offset_casters, 12, &pos);
	_shadowcasters_buffer->WriteData(offset_casters + 12, 4, &type);

	_added_casters++;
}

void VulkanShadowmapping::ResizeOutput(uint32 width, uint32 height) {
	_shadowprocess_framebuffer->SetSize(width, height);
}

VulkanTexture* VulkanShadowmapping::GetOutputTexture() {
	return (VulkanTexture*)_shadowprocess_framebuffer->GetColorAttachments()[0];
}

void VulkanShadowmapping::ResetCasters() {
	_added_casters = 0;
}

void VulkanShadowmapping::ProcessShadowCaster(uint32 casterIndex) {
	VulkanShadowCaster* caster = _casters[casterIndex];
	uint32 cascades = caster->_framebuffer->GetLayersCount();

	VulkanCommandBuffer* cmdbuf = caster->_cmdbuf;
	VulkanFramebuffer* fb = caster->_framebuffer;

	uint32 vertexOffsets[] = { 0, 0 };
	uint32 shadowmap_offset = casterIndex * SHADOWMAP_BUFFER_ELEMENT_SIZE;

	_writtenBones = 0;
	_writtenParticleTransforms = 0;

	cmdbuf->Begin();
	
	if (caster->_caster_type == LIGHT_TYPE_DIRECTIONAL) {
		_shadowmapRenderPass->CmdBegin(*cmdbuf, *fb);
		cmdbuf->BindPipeline(*_shadowmapPipeline);
		cmdbuf->SetViewport(0, 0, MAP_SIZE, MAP_SIZE);
		cmdbuf->BindDescriptorSets(*_shadowmap_layout, 1, 1, _shadowcaster_descrSet, 1, &shadowmap_offset);
	}else if (caster->_caster_type == LIGHT_TYPE_POINT) {
		_shadowmap_point_RenderPass->CmdBegin(*cmdbuf, *fb);
		cmdbuf->BindPipeline(*_shadowmap_point_Pipeline);
		cmdbuf->SetViewport(0, 0, MAP_SIZE, MAP_SIZE);
		cmdbuf->BindDescriptorSets(*_shadowmap_layout, 1, 1, _shadowcaster_descrSet, 1, &shadowmap_offset);
	}

	for (uint32 e_i = 0; e_i < _entitiesToRender->size(); e_i++) {
		Entity* entity = _entitiesToRender->at(e_i);
		MeshResource* mesh_resource = entity->GetComponent<MeshComponent>()->GetMeshResource();

		if (mesh_resource->GetState() == RESOURCE_STATE_READY) {
			VulkanMesh* mesh = (VulkanMesh*)mesh_resource->GetMesh();
			//Mark mesh resource used in this frame
			mesh_resource->Use();

			uint32 offsets[2] = { 0, e_i * UNI_ALIGN };
			uint32 anim_offset = _writtenBones * 64;
			_writtenBones += mesh->GetBones().size();

			int vertexDescriptorID = (e_i * UNI_ALIGN) / 65535;

			cmdbuf->BindDescriptorSets(*_shadowmap_layout, 0, 1, _vertexDescrSets->at(vertexDescriptorID), 2, offsets);
			cmdbuf->BindDescriptorSets(*_shadowmap_layout, 2, 1, animsDescrSet, 1, &anim_offset);
			cmdbuf->BindMesh(*mesh);
			if (mesh->GetIndexCount() > 0)
				cmdbuf->DrawIndexed(mesh->GetIndexCount());
			else
				cmdbuf->Draw(mesh->GetVerticesCount());
		}

		if (_writtenBones % 4 > 0) {
			_writtenBones += 4 - (_writtenBones % 4);
		}
	}

	cmdbuf->EndRenderPass();
	cmdbuf->End();
}

void VulkanShadowmapping::ExecuteShadowCaster(uint32 casterIndex, VulkanSemaphore* begin, VulkanSemaphore* end) {
	
	VulkanShadowCaster* caster = _casters[casterIndex];
	VulkanCommandBuffer* cmdbuf = caster->_cmdbuf;

	if (begin == nullptr)
		begin = _shadowmapFirstSemaphore;

	if (end == nullptr)
		end = _shadowmapSecondSemaphore;

	VulkanGraphicsSubmit(*cmdbuf, *begin, *end);

	VulkanSemaphore* temp = _shadowmapFirstSemaphore;

	_shadowmapFirstSemaphore = _shadowmapSecondSemaphore;
	_shadowmapSecondSemaphore = temp;

}

void VulkanShadowmapping::RecordShadowProcessingCmdbuf() {
	_shadowprocess_cmdbuf->Begin();
	_shadowprocessRenderPass->CmdBegin(*_shadowprocess_cmdbuf, *_shadowprocess_framebuffer);
	_shadowprocess_cmdbuf->BindPipeline(*_shadowprocess_pipeline);
	_shadowprocess_cmdbuf->SetViewport(0, 0, 1280, 720);
	_shadowprocess_cmdbuf->BindDescriptorSets(*_shadowprocess_layout, 0, 1, _shadowrenderer_descrSet);
	_shadowprocess_cmdbuf->BindMesh(*_screenPlane);
	_shadowprocess_cmdbuf->DrawIndexed(6);
	_shadowprocess_cmdbuf->EndRenderPass();
	_shadowprocess_cmdbuf->End();
}

void VulkanShadowmapping::RenderShadows(VulkanSemaphore* begin, VulkanSemaphore* end) {

	//Write shadowmaps to descriptor set
	std::vector<VulkanTexture*> shadowmaps;
	std::vector<VulkanTexture*> shadowmaps_point;
	for (auto caster : _casters) {
		if (caster->_caster_type == LIGHT_TYPE_POINT)
			shadowmaps_point.push_back((VulkanTexture*)caster->_framebuffer->GetColorAttachments()[0]);
		else
			shadowmaps.push_back((VulkanTexture*)caster->_framebuffer->GetDepthAttachment());
	}

	_shadowrenderer_descrSet->WriteDescriptorImages(3, shadowmaps.data(), _shadowmap_sampler, shadowmaps.size());
	_shadowrenderer_descrSet->WriteDescriptorImages(4, shadowmaps_point.data(), _shadowmap_sampler, shadowmaps_point.size());
	_shadowprocess_buffer->WriteData(43008, 4, &_added_casters);
	RecordShadowProcessingCmdbuf();


	if(_scene){
		SceneEnvironmentSettings& env_settings = _scene->GetEnvironmentSettings();
		uint32 cascades_count = env_settings.GetShadowCascadesCount();
		for(uint32 cascade = 0; cascade < cascades_count; cascade ++){
			float cascade_depth = env_settings.GetCascadeDepths()[cascade];
			_cascadeinfo_buffer->WriteData(16 * cascade, 4, &cascade_depth);
		}
		_cascadeinfo_buffer->WriteData(160, 4, &cascades_count);
		int map_size = MAP_SIZE;
		_cascadeinfo_buffer->WriteData(164, 4, &map_size);
	}

	VulkanGraphicsSubmit(*_shadowprocess_cmdbuf, *begin, *end);
}