#include "VulkanGammaCorrection.hpp"
#include "../VulkanRenderer.hpp"

using namespace VSGE;

VulkanGammaCorrection::VulkanGammaCorrection() {
	_shader = nullptr;
	_descr_pool = nullptr;
	_descr_set = nullptr;
	_pp_layout = nullptr;
	_pipeline = nullptr;

	_output_sizes = Vec2i(1280, 720);
}
VulkanGammaCorrection::~VulkanGammaCorrection() {
	Destroy();
}

void VulkanGammaCorrection::Create() {
	_shader = new VulkanShader;
	_shader->AddShaderFromFile("gamma_correction.comp", SHADER_STAGE_COMPUTE);

	_descr_pool = new VulkanDescriptorPool();
	_descr_pool->SetDescriptorSetsCount(1);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1);
	_descr_pool->Create();

	_descr_set = new VulkanDescriptorSet();
	_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
	_descr_set->SetDescriptorPool(_descr_pool);
	_descr_set->Create();

	_pp_layout = new VulkanPipelineLayout;
	_pp_layout->PushDescriptorSet(_descr_set);
	_pp_layout->Create();

	_pipeline = new VulkanComputePipeline;
	_pipeline->Create(_shader, _pp_layout);
	//Create output texture
	_output = new VulkanTexture;
	_output->SetStorage(true);
	_output->Create(1280, 720, FORMAT_RGBA, 1, 1);

	_descr_set->WriteDescriptorImage(1, (VulkanTexture*)_output, nullptr, VK_IMAGE_LAYOUT_GENERAL);
}
void VulkanGammaCorrection::Destroy() {
	SAFE_RELEASE(_pipeline)
	
}

void VulkanGammaCorrection::SetInputTexture(Texture* input) {
	PostprocessEffect::SetInputTexture(input);
	
	_descr_set->WriteDescriptorImage(0, (VulkanTexture*)input, VulkanRenderer::Get()->GetAttachmentSampler());
}

void VulkanGammaCorrection::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {
	VkImageMemoryBarrier pre_barrier = GetImageBarrier((VulkanTexture*)_output, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_arrier = GetImageBarrier((VulkanTexture*)_output, VK_ACCESS_SHADER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, { pre_barrier });
	
	cmdbuf->BindComputePipeline(*_pipeline);
	cmdbuf->BindDescriptorSets(*_pp_layout, 0, 1, _descr_set, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
	cmdbuf->Dispatch(_output_sizes.x / 32 + 1, _output_sizes.y / 32	+ 1, 1);

	cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { post_arrier });
}

void VulkanGammaCorrection::ResizeOutput(const Vec2i& new_size) {
	if (_output_sizes == new_size)
		return;

	_output_sizes = new_size;
	_output->Resize(new_size.x, new_size.y);
	SetInputTexture(_output);

	_descr_set->WriteDescriptorImage(1, (VulkanTexture*)_output, nullptr, VK_IMAGE_LAYOUT_GENERAL);
}