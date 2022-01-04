#include "VulkanBloom.hpp"
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

using namespace VSGE;

VulkanBloomBase* base = nullptr;

VulkanBloomBase::VulkanBloomBase() {
	Create();
}
VulkanBloomBase::~VulkanBloomBase() {
	Destroy();
}
void VulkanBloomBase::Create(){
	_bloom_bright_shader = new VulkanShader;
	_bloom_bright_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
	_bloom_bright_shader->AddShaderFromFile("bloom_bright.frag", SHADER_STAGE_FRAGMENT);

	_bloom_blur_v_shader = new VulkanShader;
	_bloom_blur_v_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
	_bloom_blur_v_shader->AddShaderFromFile("bloom_blur_v.frag", SHADER_STAGE_FRAGMENT);

	_bloom_blur_h_shader = new VulkanShader;
	_bloom_blur_h_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
	_bloom_blur_h_shader->AddShaderFromFile("bloom_blur_h.frag", SHADER_STAGE_FRAGMENT);


	VulkanDescriptorSet* base_set = new VulkanDescriptorSet;
	base_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	base_set->CreateLayout();

	_bloom_bright_playout = new VulkanPipelineLayout;
	_bloom_bright_playout->PushDescriptorSet(base_set);
	_bloom_bright_playout->Create();

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	VulkanRenderPass* bright_rp = CreateBrightRenderpass();

	_bloom_bright_pipeline = new VulkanPipeline;
	_bloom_bright_pipeline->SetDynamicCullMode(false);
	_bloom_bright_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_bloom_bright_pipeline->Create(_bloom_bright_shader, bright_rp, _vertexLayout, _bloom_bright_playout);
	
	_bloom_blur_v_pipeline = new VulkanPipeline;
	_bloom_blur_v_pipeline->SetDynamicCullMode(false);
	_bloom_blur_v_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_bloom_blur_v_pipeline->Create(_bloom_blur_v_shader, bright_rp, _vertexLayout, _bloom_bright_playout);

	_bloom_blur_h_pipeline = new VulkanPipeline;
	_bloom_blur_h_pipeline->SetDynamicCullMode(false);
	_bloom_blur_h_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
	_bloom_blur_h_pipeline->Create(_bloom_blur_h_shader, bright_rp, _vertexLayout, _bloom_bright_playout);

	delete bright_rp;
	delete base_set;
}

VulkanRenderPass* VulkanBloomBase::CreateBrightRenderpass() {
	VulkanRenderPass* bright_rp = new VulkanRenderPass;
	bright_rp->PushColorAttachment(FORMAT_RGBA16F);
	bright_rp->Create();
	return bright_rp;
}

VulkanPipeline* VulkanBloomBase::GetBrightnessPipeline() {
	return _bloom_bright_pipeline;
}
VulkanPipeline* VulkanBloomBase::GetBlurVPipeline() {
	return _bloom_blur_v_pipeline;
}
VulkanPipeline* VulkanBloomBase::GetBlurHPipeline() {
	return _bloom_blur_h_pipeline;
}
VulkanPipelineLayout* VulkanBloomBase::GetBrightnessPipelineLayout() {
	return _bloom_bright_playout;
}

void VulkanBloomBase::Destroy() {
	SAFE_RELEASE(_bloom_blur_h_pipeline);
	SAFE_RELEASE(_bloom_blur_v_pipeline);
	SAFE_RELEASE(_bloom_bright_pipeline);
}


VulkanBloom::VulkanBloom() {

}
VulkanBloom::~VulkanBloom() {

}
void VulkanBloom::Create() {
	if(!base)
		base = new VulkanBloomBase();

	_descr_pool = new VulkanDescriptorPool;

	_bright_descr_set = new VulkanDescriptorSet(_descr_pool);
	_bright_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	
	_blur_1_descr_set = new VulkanDescriptorSet(_descr_pool);
	_blur_1_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);

	_blur_2_descr_set = new VulkanDescriptorSet(_descr_pool);
	_blur_2_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	
	_descr_pool->Create();
	_bright_descr_set->Create();
	_blur_1_descr_set->Create();
	_blur_2_descr_set->Create();

	_rpass = base->CreateBrightRenderpass();
	_rpass->SetClearSize(1280, 720);

	_fb_bright = new VulkanFramebuffer;
	_fb_bright->SetSize(1280, 720);
	_fb_bright->AddAttachment(FORMAT_RGBA16F);
	_fb_bright->Create(_rpass);

	_fb_1 = new VulkanFramebuffer;
	_fb_1->SetSize(1280, 720);
	_fb_1->AddAttachment(FORMAT_RGBA16F);
	_fb_1->Create(_rpass);

	_fb_2 = new VulkanFramebuffer;
	_fb_2->SetSize(1280, 720);
	_fb_2->AddAttachment(FORMAT_RGBA16F);
	_fb_2->Create(_rpass);
}
void VulkanBloom::Destroy() {

}
void VulkanBloom::SetInputTextureHDR(VulkanTexture* input) {
	_input_texture = input;
	_bright_descr_set->WriteDescriptorImage(0, input, 
		VulkanRenderer::Get()->GetAttachmentSampler());
}
VulkanTexture* VulkanBloom::GetBlurredBloomTextureHDR() {
	return (VulkanTexture*)_fb_2->GetColorAttachments()[0];
}
void VulkanBloom::RecordCommandBuffer(VulkanCommandBuffer* cmdbuf) {
	_rpass->CmdBegin(*cmdbuf, *_fb_bright);
	cmdbuf->BindPipeline(*base->GetBrightnessPipeline());
	cmdbuf->SetViewport(0, 0, (float)_output_size.x, (float)_output_size.y);
	cmdbuf->BindDescriptorSets(*base->GetBrightnessPipelineLayout(), 0, 1,
		_bright_descr_set);
	cmdbuf->BindMesh(*VulkanRenderer::Get()->GetScreenMesh(), 0);
	cmdbuf->DrawIndexed(6);
	cmdbuf->EndRenderPass();

	_rpass->CmdBegin(*cmdbuf, *_fb_1);
	cmdbuf->BindPipeline(*base->GetBlurVPipeline());
	cmdbuf->BindDescriptorSets(*base->GetBrightnessPipelineLayout(), 0, 1,
		_blur_1_descr_set);
	cmdbuf->BindMesh(*VulkanRenderer::Get()->GetScreenMesh(), 0);
	cmdbuf->DrawIndexed(6);
	cmdbuf->EndRenderPass();

	_rpass->CmdBegin(*cmdbuf, *_fb_2);
	cmdbuf->BindPipeline(*base->GetBlurHPipeline());
	cmdbuf->BindDescriptorSets(*base->GetBrightnessPipelineLayout(), 0, 1,
		_blur_2_descr_set);
	cmdbuf->BindMesh(*VulkanRenderer::Get()->GetScreenMesh(), 0);
	cmdbuf->DrawIndexed(6);
	cmdbuf->EndRenderPass();
}
void VulkanBloom::ResizeOutput(const Vec2i& new_size) {
	_output_size = new_size;
	_rpass->SetClearSize(new_size.x, new_size.y);

	_fb_bright->Resize(new_size.x, new_size.y);
	_fb_1->Resize(new_size.x, new_size.y);
	_fb_2->Resize(new_size.x, new_size.y);

	_blur_1_descr_set->WriteDescriptorImage(0,
		(VulkanTexture*)_fb_bright->GetColorAttachments()[0],
		VulkanRenderer::Get()->GetAttachmentSampler());

	_blur_2_descr_set->WriteDescriptorImage(0,
		(VulkanTexture*)_fb_1->GetColorAttachments()[0],
		VulkanRenderer::Get()->GetAttachmentSampler());
}