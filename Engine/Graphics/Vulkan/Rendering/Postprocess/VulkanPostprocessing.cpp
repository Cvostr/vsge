#include "VulkanPostprocessing.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>
#include <Graphics/Vulkan/Rendering/VulkanRenderer.hpp>

using namespace VSGE;

VulkanPostprocessing::VulkanPostprocessing() {
	_input_texture = nullptr;
	_input_depth = nullptr;
	_input_normals = nullptr;
	_input_positions = nullptr;
	_output_sizes = Vec2i(1280, 720);
}
VulkanPostprocessing::~VulkanPostprocessing() {

}

void VulkanPostprocessing::SetInputTextures(
	Texture* input,
	Texture* depth,
	Texture* normals,
	Texture* positions,
	Texture* ui)
{
	this->_input_texture = input;
	this->_input_depth = depth;
	this->_input_normals = normals;
	this->_input_positions = positions;
	this->_input_ui = ui;
}

void VulkanPostprocessing::Create() {
	_gamma_correction = new VulkanGammaCorrection;
	_gamma_correction->Create();

	_bloom = new VulkanBloom;
	_bloom->Create();

	_ssao = new VulkanSSAO;
	_ssao->Create();

	_cmdpool = new VulkanCommandPool;
	_cmdpool->Create(VulkanRAPI::Get()->GetDevice()->GetComputeQueueFamilyIndex());

	_cmdbuf = new VulkanCommandBuffer;
	_cmdbuf->Create(_cmdpool);

	_begin_semaphore = new VulkanSemaphore;
	_begin_semaphore->Create();

	//----UI adding
	_output_texture = new VulkanTexture;
	_output_texture->SetStorage(true);
	_output_texture->Create(_output_sizes.x, _output_sizes.y);

	_ui_add_shader = new VulkanShader;
	_ui_add_shader->AddShaderFromFile("ui_add.comp", SHADER_STAGE_COMPUTE);

	_ui_descr_pool = new VulkanDescriptorPool;
	_ui_descr_pool->SetDescriptorSetsCount(1);
	_ui_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3);
	_ui_descr_pool->Create();

	_ui_descr_set = new VulkanDescriptorSet;
	_ui_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_ui_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
	_ui_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2, VK_SHADER_STAGE_COMPUTE_BIT);
	_ui_descr_set->SetDescriptorPool(_ui_descr_pool);
	_ui_descr_set->Create();

	_ui_descr_set->WriteDescriptorImage(2, _output_texture, nullptr, VK_IMAGE_LAYOUT_GENERAL);

	_ui_add_pl_layout = new VulkanPipelineLayout;
	_ui_add_pl_layout->PushDescriptorSet(_ui_descr_set);
	_ui_add_pl_layout->Create();

	_ui_add_pipeline = new VulkanComputePipeline;
	_ui_add_pipeline->Create(_ui_add_shader, _ui_add_pl_layout);
}
void VulkanPostprocessing::Destroy() {
	SAFE_RELEASE(_begin_semaphore)
	SAFE_RELEASE(_cmdbuf);
	SAFE_RELEASE(_cmdpool);

	SAFE_RELEASE(_gamma_correction)
	SAFE_RELEASE(_bloom);
	SAFE_RELEASE(_ssao);
}

void VulkanPostprocessing::FillCommandBuffer() {
	_ui_descr_set->WriteDescriptorImage(0, 
		(VulkanTexture*)_gamma_correction->GetOutputTexture(), 
		nullptr,
		VK_IMAGE_LAYOUT_GENERAL);
	_ui_descr_set->WriteDescriptorImage(1,
		(VulkanTexture*)_input_ui,
		nullptr,
		VK_IMAGE_LAYOUT_GENERAL);

	_bloom->SetInputTexture(_input_texture);
	_gamma_correction->SetInputTexture(_input_texture);

	_cmdbuf->Begin();
	_bloom->FillCommandBuffer(_cmdbuf);
	_gamma_correction->FillCommandBuffer(_cmdbuf);
	//UI adding
	VkImageMemoryBarrier pre_barrier = GetImageBarrier(_output_texture, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_barrier = GetImageBarrier(_output_texture, VK_ACCESS_SHADER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageMemoryBarrier pre_barrier_world = GetImageBarrier((VulkanTexture*)_gamma_correction->GetOutputTexture(), 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_barrier_world = GetImageBarrier((VulkanTexture*)_gamma_correction->GetOutputTexture(), VK_ACCESS_SHADER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageMemoryBarrier pre_barrier_ui = GetImageBarrier((VulkanTexture*)_input_ui, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_barrier_ui = GetImageBarrier((VulkanTexture*)_input_ui, VK_ACCESS_SHADER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	
	_cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, { pre_barrier, pre_barrier_world, pre_barrier_ui });
	_cmdbuf->BindComputePipeline(*_ui_add_pipeline);
	_cmdbuf->BindDescriptorSets(*_ui_add_pl_layout, 0, 1, _ui_descr_set, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
	_cmdbuf->Dispatch(_output_sizes.x / 32 + 1, _output_sizes.y / 32 + 1, 1);
	_cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { post_barrier, post_barrier_world, post_barrier_ui });
	
	_cmdbuf->End();
}

VulkanTexture* VulkanPostprocessing::GetOutputTexture() {
	return _output_texture;
}
VulkanSemaphore* VulkanPostprocessing::GetBeginSemaphore() {
	return _begin_semaphore;
}
void VulkanPostprocessing::Execute(VulkanSemaphore* end_semaphore) {
	VulkanComputeSubmit(*_cmdbuf, *_begin_semaphore, *end_semaphore);
}
void VulkanPostprocessing::ResizeOutput(const Vec2i& new_size) {
	if (_output_sizes == new_size)
		return;

	_output_sizes = new_size;
	//resize output
	_output_texture->Resize(_output_sizes.x, _output_sizes.y);
	_ui_descr_set->WriteDescriptorImage(2, _output_texture, nullptr, VK_IMAGE_LAYOUT_GENERAL);

	_gamma_correction->ResizeOutput(new_size);
	_bloom->ResizeOutput(new_size);
}