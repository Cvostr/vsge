#include "VulkanGammaCorrection.hpp"
#include "../VulkanRenderer.hpp"

using namespace VSGE;

VulkanShader* gamma_shader = nullptr;
VulkanPipelineLayout* gamma_pl_layout = nullptr;
VulkanPipeline* gamma_pipeline = nullptr;

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
	CreateDescriptors();
	if (!gamma_pl_layout) {
		gamma_pl_layout = new VulkanPipelineLayout;
		gamma_pl_layout->PushDescriptorSet(_descr_set);
		gamma_pl_layout->Create();
	}
	if (!gamma_pipeline) {
		VulkanRenderPass* gamma_rp = new VulkanRenderPass;
		gamma_rp->PushColorAttachment(FORMAT_RGBA);
		gamma_rp->SetAttachmentClearOnLoad(0, false);
		gamma_rp->SetClearSize(10, 10);
		gamma_rp->Create();

		VertexLayout _vertexLayout;
		_vertexLayout.AddBinding(sizeof(Vertex));
		_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
		_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

		gamma_pipeline = new VulkanPipeline;
		gamma_pipeline->SetDynamicCullMode(false);
		gamma_pipeline->SetCullMode(CullMode::CULL_MODE_BACK);
		gamma_pipeline->Create(gamma_shader, gamma_rp, _vertexLayout, gamma_pl_layout);
	}
}

void VulkanGammaCorrection::CreateDescriptors() {
	_pool = new VulkanDescriptorPool;

	_descr_set = new VulkanDescriptorSet(_pool);
	_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	_pool->Create();
	_descr_set->Create();
}

void VulkanGammaCorrection::Create() {
	CreateConstants();

	_rp = new VulkanRenderPass;
	_rp->PushColorAttachment(FORMAT_RGBA);
	_rp->SetAttachmentClearOnLoad(0, false);
	_rp->SetClearSize(10, 10);
	_rp->Create();

	_fb = new VulkanFramebuffer;
	_fb->SetSize(_output_size.x, _output_size.y);
	_fb->AddAttachment();
	_fb->Create(_rp);
}
void VulkanGammaCorrection::Destroy() {
	_fb->Destroy();
}
void VulkanGammaCorrection::SetInputTexture(Texture * input) {
	_input = input;

	_descr_set->WriteDescriptorImage(0, (VulkanTexture*)input,
		VulkanRenderer::Get()->GetAttachmentSampler());
}
VulkanTexture* VulkanGammaCorrection::GetOutputTexture() {
	return (VulkanTexture*)_fb->GetColorAttachments()[0];
}
void VulkanGammaCorrection::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {
	_rp->CmdBegin(*cmdbuf, *_fb);
	cmdbuf->BindPipeline(*gamma_pipeline);
	cmdbuf->SetCullMode(VK_CULL_MODE_NONE);
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