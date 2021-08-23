#include "VulkanSpecularMap.hpp"
#include "../../VulkanRAPI.hpp"

using namespace VSGE;

void VulkanSpecularMap::Create() {
	_spmap_shader = new VulkanShader;
	_spmap_shader->AddShaderFromFile("spmap.comp", SHADER_STAGE_COMPUTE);

	_spmap_output_texture = new VulkanTexture;
	_spmap_output_texture->SetCubemap(true);
	_spmap_output_texture->SetStorage(true);
	_spmap_output_texture->Create(1024, 1024, FORMAT_RGBA16F, 6, 1);
	_spmap_output_texture->CreateImageView();

	_spmap_descr_pool = new VulkanDescriptorPool;
	_spmap_descr_pool->SetDescriptorSetsCount(1);
	_spmap_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2);
	_spmap_descr_pool->Create();

	_spmap_descr_set = new VulkanDescriptorSet(_spmap_descr_pool);
	_spmap_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
	_spmap_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
	_spmap_descr_set->Create();
	_spmap_descr_set->WriteDescriptorImage(1, _spmap_output_texture, nullptr, VK_IMAGE_LAYOUT_GENERAL);


	_spmap_pipeline_layout = new VulkanPipelineLayout;
	_spmap_pipeline_layout->PushDescriptorSet(_spmap_descr_set);
	_spmap_pipeline_layout->Create();

	_spmap_pipeline = new VulkanComputePipeline;
	_spmap_pipeline->Create(_spmap_shader, _spmap_pipeline_layout);

	VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

	_spmap_cmdpool = new VulkanCommandPool;
	_spmap_cmdpool->Create(device->GetComputeQueueFamilyIndex());

	_spmap_cmdbuffer = new VulkanCommandBuffer();
	_spmap_cmdbuffer->Create(_spmap_cmdpool);
}

void VulkanSpecularMap::FillCommandBuffer() {

}