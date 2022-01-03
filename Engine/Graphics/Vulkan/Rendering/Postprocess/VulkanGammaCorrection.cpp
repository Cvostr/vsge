#include "VulkanGammaCorrection.hpp"
#include "../VulkanRenderer.hpp"

using namespace VSGE;

VulkanShader* gamma_shader = nullptr;
VulkanShader* gamma_shader_alpha = nullptr;
VulkanPipelineLayout* gamma_pl_layout = nullptr;
VulkanPipeline* gamma_pipeline = nullptr;
VulkanPipeline* gamma_pipeline_alpha = nullptr;

VulkanGammaCorrection::VulkanGammaCorrection() {
	_output_size = Vec2i(1280, 720);
	Create();
}
VulkanGammaCorrection::~VulkanGammaCorrection() {
	Destroy();
}

void VulkanGammaCorrection::CreateConstants() {
	if (!gamma_shader) {
		gamma_shader = new VulkanShader;
		gamma_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
		gamma_shader->AddShaderFromFile("gamma_correction.frag", SHADER_STAGE_FRAGMENT);
	}
	if (!gamma_shader_alpha) {
		gamma_shader_alpha = new VulkanShader;
		gamma_shader_alpha->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
		gamma_shader_alpha->AddShaderFromFile("gamma_correction_alpha.frag", SHADER_STAGE_FRAGMENT);
	}
	CreateDescriptors();
	if (!gamma_pl_layout) {
		gamma_pl_layout = new VulkanPipelineLayout;
		gamma_pl_layout->PushDescriptorSet(_descr_set);
		gamma_pl_layout->Create();
	}
	if (!gamma_pipeline) {
		VulkanRenderPass* gamma_rp = new VulkanRenderPass;
		gamma_rp->PushColorAttachment(FORMAT_RGBA);
		gamma_rp->Create();

		VertexLayout _vertexLayout;
		_vertexLayout.AddBinding(sizeof(Vertex));
		_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
		_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

		gamma_pipeline = new VulkanPipeline;
		gamma_pipeline->SetDynamicCullMode(false);
		gamma_pipeline->SetCullMode(CullMode::CULL_MODE_FRONT);
		gamma_pipeline->Create(gamma_shader, gamma_rp, _vertexLayout, gamma_pl_layout);

		gamma_pipeline_alpha = new VulkanPipeline;
		gamma_pipeline_alpha->SetDynamicCullMode(false);
		gamma_pipeline_alpha->SetCullMode(CullMode::CULL_MODE_FRONT);
		gamma_pipeline_alpha->Create(gamma_shader_alpha, gamma_rp, _vertexLayout, gamma_pl_layout);
	
		gamma_rp->Destroy();
	}
}

void VulkanGammaCorrection::CreateDescriptors() {
	_pool = new VulkanDescriptorPool;

	_descr_set = new VulkanDescriptorSet(_pool);
	_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	_pool->Create();
	_descr_set->Create();

	SetInputBloomTexture(nullptr);
}

void VulkanGammaCorrection::Create() {
	CreateConstants();

	_pipeline = gamma_pipeline;

	_rp = new VulkanRenderPass;
	_rp->PushColorAttachment(FORMAT_RGBA);
	_rp->Create();

	_fb = new VulkanFramebuffer;
	_fb->SetSize(_output_size.x, _output_size.y);
	_fb->AddAttachment();
	_fb->Create(_rp);
}
void VulkanGammaCorrection::Destroy() {
	_fb->Destroy();
	_rp->Destroy();

	_descr_set->Destroy();
	_pool->Destroy();
}
void VulkanGammaCorrection::SetKeepAlpha(bool keepAlpha) {
	_pipeline = (keepAlpha ? gamma_pipeline_alpha : gamma_pipeline);
}
void VulkanGammaCorrection::SetInputTexture(VulkanTexture * input) {
	_input = input;

	_descr_set->WriteDescriptorImage(0, input,
		VulkanRenderer::Get()->GetAttachmentSampler());
}
void VulkanGammaCorrection::SetInputBloomTexture(VulkanTexture* input) {
	if (!input)
		input = VulkanRenderer::Get()->GetBlackTexture();
	_descr_set->WriteDescriptorImage(1, input,
		VulkanRenderer::Get()->GetAttachmentSampler());
}
VulkanTexture* VulkanGammaCorrection::GetOutputTexture() {
	return (VulkanTexture*)_fb->GetColorAttachments()[0];
}
void VulkanGammaCorrection::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {
	_rp->CmdBegin(*cmdbuf, *_fb);
	cmdbuf->BindPipeline(*_pipeline);
	cmdbuf->SetViewport(0, 0, (float)_output_size.x, (float)_output_size.y);
	cmdbuf->BindDescriptorSets(*gamma_pl_layout, 0, 1, _descr_set, 0);
	cmdbuf->BindMesh(*VulkanRenderer::Get()->GetScreenMesh(), 0);
	cmdbuf->DrawIndexed(6);
	cmdbuf->EndRenderPass();
}
void VulkanGammaCorrection::ResizeOutput(const Vec2i& new_size) {
	_output_size = new_size;
	_rp->SetClearSize(new_size.x, new_size.y);
	_fb->Resize(new_size.x, new_size.y);
}