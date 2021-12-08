#include "VulkanBloom.hpp"
#include "../VulkanRenderer.hpp"

using namespace VSGE;

VulkanBloom::VulkanBloom() {
	_output_size = Vec2i(1280, 720);
}
VulkanBloom::~VulkanBloom() {
	Destroy();
}

void VulkanBloom::Create() {
	_bloom_bright_shader = new VulkanShader;
	_bloom_bright_shader->AddShaderFromFile("bloom_bright.comp", SHADER_STAGE_COMPUTE);

	_descr_pool = new VulkanDescriptorPool;
	_descr_pool->SetDescriptorSetsCount(2);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
	_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2);
	_descr_pool->Create();

	_bright_descr_set = new VulkanDescriptorSet;
	_bright_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_bright_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
	_bright_descr_set->SetDescriptorPool(_descr_pool);
	_bright_descr_set->Create();

	_bright_pp_layout = new VulkanPipelineLayout;
	_bright_pp_layout->PushDescriptorSet(_bright_descr_set);
	_bright_pp_layout->Create();

	_bright_pipeline = new VulkanComputePipeline;
	_bright_pipeline->Create(_bloom_bright_shader, _bright_pp_layout);

	_bright_texture = new VulkanTexture;
	_bright_texture->SetStorage(true);
	_bright_texture->Create(1280, 720, FORMAT_RGBA16F, 1, 1);

	_output = new VulkanTexture;
	_output->SetStorage(true);
	_output->Create(1280, 720, FORMAT_RGBA16F, 1, 1);
}
void VulkanBloom::Destroy() {

}

void VulkanBloom::SetInputTexture(Texture* input) {
	PostprocessEffect::SetInputTexture(input);

	_bright_descr_set->WriteDescriptorImage(0, (VulkanTexture*)input, VulkanRenderer::Get()->GetAttachmentSampler());
	_bright_descr_set->WriteDescriptorImage(1, _bright_texture, nullptr, VK_IMAGE_LAYOUT_GENERAL);
}
void VulkanBloom::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {
	VkImageMemoryBarrier pre_barrier_in = GetImageBarrier((VulkanTexture*)_input,
		0, VK_ACCESS_SHADER_WRITE_BIT,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_barrier_in = GetImageBarrier((VulkanTexture*)_input, 
		VK_ACCESS_SHADER_WRITE_BIT, 0, 
		VK_IMAGE_LAYOUT_GENERAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageMemoryBarrier pre_barrier_bright = GetImageBarrier((VulkanTexture*)_bright_texture,
		0, VK_ACCESS_SHADER_WRITE_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_GENERAL);
	VkImageMemoryBarrier post_barrier_bright = GetImageBarrier((VulkanTexture*)_bright_texture,
		VK_ACCESS_SHADER_WRITE_BIT, 0,
		VK_IMAGE_LAYOUT_GENERAL, 
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	
	cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, { pre_barrier_in, post_barrier_bright });
	cmdbuf->BindComputePipeline(*_bright_pipeline);
	cmdbuf->BindDescriptorSets(*_bright_pp_layout, 0, 1, _bright_descr_set, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
	cmdbuf->Dispatch(_output_size.x / 32 + 1, _output_size.y / 32 + 1, 1);
	cmdbuf->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { post_barrier_bright, post_barrier_in });

}

void VulkanBloom::ResizeOutput(const Vec2i& new_size) {
	if (_output_size == new_size)
		return;

	_output_size = new_size;
	_bright_texture->Resize(new_size.x, new_size.y);
	_output->Resize(new_size.x, new_size.y);

	_bright_descr_set->WriteDescriptorImage(1, (VulkanTexture*)_bright_texture,
		nullptr, VK_IMAGE_LAYOUT_GENERAL);
}