#include "VulkanDeferredLight.hpp"
#include "VulkanRenderer.hpp"

using namespace VSGE;

VulkanDefferedLight::VulkanDefferedLight() {
	_fb_width = 1280;
	_fb_height = 720;
}

VulkanDefferedLight::~VulkanDefferedLight() {
	SAFE_RELEASE(_deferred_pipeline)

	SAFE_RELEASE(_deferred_fb)
	SAFE_RELEASE(_deferred_rp)
}

void VulkanDefferedLight::CreateFramebuffer() {
	_deferred_rp = new VulkanRenderPass;
	_deferred_rp->SetClearSize(_fb_width, _fb_height);
	_deferred_rp->PushColorAttachment(FORMAT_RGBA);
	_deferred_rp->Create();

	_deferred_fb = new VulkanFramebuffer;
	_deferred_fb->SetSize(_fb_width, _fb_height);
	_deferred_fb->AddAttachment(FORMAT_RGBA);
	_deferred_fb->Create(_deferred_rp);
}

void VulkanDefferedLight::CreateDescriptorSet(){
	//Allocate pool
	_deferred_pool = new VulkanDescriptorPool;

	_deferred_descriptor = new VulkanDescriptorSet(_deferred_pool);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, VK_SHADER_STAGE_FRAGMENT_BIT);
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6, VK_SHADER_STAGE_FRAGMENT_BIT);
	//shadows
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 7, VK_SHADER_STAGE_FRAGMENT_BIT);
	//depth
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8, VK_SHADER_STAGE_FRAGMENT_BIT);
	//brdf lut
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 9, VK_SHADER_STAGE_FRAGMENT_BIT);
	//specular env map
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10, VK_SHADER_STAGE_FRAGMENT_BIT);
	//irradiance env map
	_deferred_descriptor->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 11, VK_SHADER_STAGE_FRAGMENT_BIT);

	_deferred_pool->Create();
	_deferred_descriptor->Create();
}

void VulkanDefferedLight::CreatePipeline() {
	_deferred_pipeline_layout = new VulkanPipelineLayout;
	_deferred_pipeline_layout->PushDescriptorSet(_deferred_descriptor);
	_deferred_pipeline_layout->Create();

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	_deferred_pipeline = new VulkanPipeline;
	_deferred_pipeline->Create((VulkanShader*)ShaderCache::Get()->GetShader("Deferred"), _deferred_rp, _vertexLayout, _deferred_pipeline_layout);
}

void VulkanDefferedLight::SetLightsBuffer(VulkanBuffer* lights_buffer) {
	_lights_buffer = lights_buffer;
	_deferred_descriptor->WriteDescriptorBuffer(2, lights_buffer);
}

void VulkanDefferedLight::SetCamerasBuffer(VulkanBuffer* cam_buffer) {
	_cam_buffer = cam_buffer;
	_deferred_descriptor->WriteDescriptorBuffer(1, cam_buffer);
}

void VulkanDefferedLight::SetGBuffer(VulkanGBufferRenderer* gbuffer) {
	if (!_deferred_descriptor)
		return;

	VulkanSampler* attachment_sampler = VulkanRenderer::Get()->GetAttachmentSampler();

	_deferred_descriptor->WriteDescriptorImage(3, gbuffer->GetAlbedoAttachment(), attachment_sampler);
	_deferred_descriptor->WriteDescriptorImage(4, gbuffer->GetNormalAttachment(), attachment_sampler);
	_deferred_descriptor->WriteDescriptorImage(5, gbuffer->GetPositionAttachment(), attachment_sampler);
	_deferred_descriptor->WriteDescriptorImage(6, gbuffer->GetMaterialsAttachment(), attachment_sampler);
	//depth
	_deferred_descriptor->WriteDescriptorImage(8, gbuffer->GetDepthAttachment(), attachment_sampler);
}

void VulkanDefferedLight::SetShadowmapper(VulkanShadowmapping* shadowmapping) {
	if (!_deferred_descriptor)
		return;

	VulkanSampler* attachment_sampler = VulkanRenderer::Get()->GetAttachmentSampler();

	_deferred_descriptor->WriteDescriptorImage(7, shadowmapping->GetOutputTexture(), attachment_sampler);
}

void VulkanDefferedLight::RecordCmdbuf(VulkanCommandBuffer* cmdbuf) {
	VulkanMesh* mesh = VulkanRenderer::Get()->GetScreenMesh();

	_deferred_rp->CmdBegin(*cmdbuf, *_deferred_fb);
	//DrawSkybox(mLightsCmdbuf);
	cmdbuf->BindPipeline(*_deferred_pipeline);
	cmdbuf->SetViewport(0, 0, _fb_width, _fb_height);
	cmdbuf->BindDescriptorSets(*_deferred_pipeline_layout, 0, 1, _deferred_descriptor);
	cmdbuf->BindMesh(*mesh, 0);
	cmdbuf->DrawIndexed(6);

	cmdbuf->EndRenderPass();
}