#include "VulkanBloom.hpp"

using namespace VSGE;

VulkanBloom::VulkanBloom() {

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
}
void VulkanBloom::Destroy() {

}

void VulkanBloom::SetInputTexture(Texture* input) {
	PostprocessEffect::SetInputTexture(input);

	//_descr_set->WriteDescriptorImage(0, (VulkanTexture*)input, VulkanRenderer::Get()->GetAttachmentSampler());
}
void VulkanBloom::FillCommandBuffer(VulkanCommandBuffer* cmdbuf) {

}