#include "VulkanFinalPass.hpp"
#include "VulkanRenderer.hpp"
#include <Graphics/Vulkan/VulkanSwapchain.hpp>
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include <Graphics/Vulkan/Rendering/VulkanRenderingGenerics.hpp>

using namespace VSGE;

VulkanFinalPass::VulkanFinalPass() :
	_rp(nullptr),
	_copy_shader(nullptr),
	_render_to_swapchain(true),
	_draw_ui(false)
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
	_copy_shader->AddShaderFromFile("ui_combine.frag", SHADER_STAGE_FRAGMENT);

	_pool = new VulkanDescriptorPool;
	_pool->SetDescriptorSetsCount(2);
	_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
	_pool->Create();

	_world_descr = new VulkanDescriptorSet;
	_world_descr->SetDescriptorPool(_pool);
	_world_descr->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_FRAGMENT_BIT); 
	_world_descr->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	_world_descr->Create();

	_copy_pl = new VulkanPipelineLayout;
	_copy_pl->PushDescriptorSet(_world_descr);
	_copy_pl->Create();

	VertexLayout _vertexLayout;
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);

	_copy_pipeline = new VulkanPipeline;
	_copy_pipeline->SetDepthTest(false);
	_copy_pipeline->Create(_copy_shader, _rp, _vertexLayout, _copy_pl);
}

void VulkanFinalPass::Destroy() {

}
void VulkanFinalPass::CreateFramebuffer() {
	_fb = new VulkanFramebuffer;
	_fb->SetSize(1280, 720);
	_fb->AddAttachment(FORMAT_RGBA);
	_fb->Create(_rp);
}
void VulkanFinalPass::SetRenderToSwapchain(bool sw) {
	_render_to_swapchain = sw;
}
void VulkanFinalPass::SetFramebuffer(VulkanFramebuffer* fb) {
	_fb = fb;
}
VulkanFramebuffer* VulkanFinalPass::GetFramebuffer() {
	return _fb;
}
void VulkanFinalPass::Resize(uint32 width, uint32 height) {
	_width = width;
	_height = height;

	_rp->SetClearSize(width, height);
	if (!_render_to_swapchain)
		_fb->Resize(width, height);
}
void VulkanFinalPass::SetAttachments(VulkanTexture* world, VulkanTexture* ui) {
	_world_descr->WriteDescriptorImage(0, world, VulkanRenderingGenerics::Get()->GetAttachmentSampler());
	_world_descr->WriteDescriptorImage(1, ui, VulkanRenderingGenerics::Get()->GetAttachmentSampler());
}
void VulkanFinalPass::RecordCmdbuffer(VulkanCommandBuffer* cmdbuffer) {
	VSGE::VulkanRAPI* vk = VSGE::VulkanRAPI::Get();
	VulkanMesh* mesh = VulkanRenderingGenerics::Get()->GetScreenMesh();

	_rp->CmdBegin(*cmdbuffer, *_fb);

	cmdbuffer->BindPipeline(*_copy_pipeline);
	cmdbuffer->SetViewport(0, 0, _width, _height);
	cmdbuffer->BindDescriptorSets(*_copy_pl, 0, 1, _world_descr);
	cmdbuffer->BindMesh(*mesh, 0);
	cmdbuffer->DrawIndexed(6);

	cmdbuffer->EndRenderPass();
}