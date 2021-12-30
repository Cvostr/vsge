#include "VulkanFinalPass.hpp"
#include "VulkanRenderer.hpp"
#include <Graphics/Vulkan/VulkanSwapchain.hpp>
#include <Graphics/Vulkan/VulkanRAPI.hpp>

using namespace VSGE;

VulkanFinalPass::VulkanFinalPass() :
	_rp(nullptr),
	_copy_shader(nullptr)
{}

VulkanFinalPass::~VulkanFinalPass() {
	Destroy();
}

void VulkanFinalPass::Create() {
	_rp = new VulkanRenderPass;
	if (_render_to_swapchain)
		_rp->PushColorOutputAttachment();
	else
		_rp->PushColorAttachment();
	_rp->Create();


	_copy_shader = new VulkanShader;
	_copy_shader->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
	_copy_shader->AddShaderFromFile("copy.frag", SHADER_STAGE_FRAGMENT);


	_pool = new VulkanDescriptorPool;
	_pool->SetDescriptorSetsCount(2);
	_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
	_pool->Create();

	_world_descr = new VulkanDescriptorSet;
	_world_descr->SetDescriptorPool(_pool);
	_world_descr->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	_world_descr->Create();

	_ui_descr = new VulkanDescriptorSet;
	_ui_descr->SetDescriptorPool(_pool);
	_ui_descr->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
	_ui_descr->Create();

	_copy_pl = new VulkanPipelineLayout;
	_copy_pl->PushDescriptorSet(_world_descr);
	_copy_pl->Create();

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	BlendAttachmentDesc particle_blend_desc;
	particle_blend_desc._blending = true;
	particle_blend_desc._srcColor = BLEND_FACTOR_SRC_ALPHA;
	particle_blend_desc._dstColor = BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

	_copy_pipeline = new VulkanPipeline;
	_copy_pipeline->SetDepthTest(false);
	_copy_pipeline->SetBlendingAttachmentDesc(0, particle_blend_desc);
	_copy_pipeline->Create(_copy_shader, _rp, _vertexLayout, _copy_pl);
}

void VulkanFinalPass::Destroy() {

}
void VulkanFinalPass::SetRenderToSwapchain(bool sw) {
	_render_to_swapchain = sw;
}
void VulkanFinalPass::SetFramebuffer(VulkanFramebuffer* fb) {
	_fb = fb;
}
void VulkanFinalPass::SetAttachments(VulkanTexture* world, VulkanTexture* ui) {
	_world_descr->WriteDescriptorImage(0, world, VulkanRenderer::Get()->GetAttachmentSampler());
	_ui_descr->WriteDescriptorImage(0, ui, VulkanRenderer::Get()->GetAttachmentSampler());
}
void VulkanFinalPass::RecordCmdbuffer(VulkanCommandBuffer* cmdbuffer) {
	VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
	VulkanSwapChain* swc = vk->GetSwapChain();
	VkExtent2D extent = swc->GetExtent();
	VulkanMesh* mesh = VulkanRenderer::Get()->GetScreenMesh();

	_rp->CmdBegin(*cmdbuffer, *_fb);

	cmdbuffer->BindPipeline(*_copy_pipeline);
	cmdbuffer->SetViewport(0, 0, extent.width, extent.height);
	cmdbuffer->BindDescriptorSets(*_copy_pl, 0, 1, _world_descr);
	cmdbuffer->BindMesh(*mesh, 0);
	cmdbuffer->DrawIndexed(6);

	cmdbuffer->BindDescriptorSets(*_copy_pl, 0, 1, _ui_descr);
	cmdbuffer->DrawIndexed(6);

	cmdbuffer->EndRenderPass();
}