#include "Vulkan_BRDF_LUT.hpp"
#include "../../VulkanRAPI.hpp"

using namespace VSGE;

Vulkan_BRDF_LUT::Vulkan_BRDF_LUT(){

}

Vulkan_BRDF_LUT::~Vulkan_BRDF_LUT(){

}

void Vulkan_BRDF_LUT::Create(){
    VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

    _brdf_lut_shader = new VulkanShader;
	_brdf_lut_shader->AddShaderFromFile("brdf_lut.comp", SHADER_STAGE_COMPUTE);

    _brdf_lut_texture = new VulkanTexture;
    _brdf_lut_texture->SetStorage(true);
    _brdf_lut_texture->Create(1024, 1024, FORMAT_RG16F);

    _brdf_descr_pool = new VulkanDescriptorPool;
    _brdf_descr_pool->SetDescriptorSetsCount(1);
    _brdf_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1);
    _brdf_descr_pool->Create();

    _brdf_descr_set = new VulkanDescriptorSet(_brdf_descr_pool);
    _brdf_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
    _brdf_descr_set->Create();
    _brdf_descr_set->WriteDescriptorImage(0, _brdf_lut_texture, nullptr, VK_IMAGE_LAYOUT_GENERAL);

    _brdf_pipeline_layout = new VulkanPipelineLayout;
    _brdf_pipeline_layout->PushDescriptorSet(_brdf_descr_set);
    _brdf_pipeline_layout->Create();

    _brdf_pipeline = new VulkanComputePipeline;
    _brdf_pipeline->Create(_brdf_lut_shader, _brdf_pipeline_layout);

    _brdf_cmdpool = new VulkanCommandPool;
    _brdf_cmdpool->Create(device->GetComputeQueueFamilyIndex());

    _brdf_cmdbuffer = new VulkanCommandBuffer();
	_brdf_cmdbuffer->Create(_brdf_cmdpool);

    VkCommandBuffer cmdbuf = _brdf_cmdbuffer->GetCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkImageMemoryBarrier pre_brdf_barrier = GetImageBarrier(_brdf_lut_texture, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    VkImageMemoryBarrier post_brdf_barrier = GetImageBarrier(_brdf_lut_texture, VK_ACCESS_SHADER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkBeginCommandBuffer(_brdf_cmdbuffer->GetCommandBuffer(), &beginInfo);
    _brdf_cmdbuffer->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, { pre_brdf_barrier });
    _brdf_cmdbuffer->BindComputePipeline(*_brdf_pipeline);
    _brdf_cmdbuffer->BindDescriptorSets(*_brdf_pipeline_layout, 0, 1, _brdf_descr_set, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
    _brdf_cmdbuffer->Dispatch(1024 / 32, 1024 / 32, 6);
    _brdf_cmdbuffer->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { post_brdf_barrier });
    vkEndCommandBuffer(_brdf_cmdbuffer->GetCommandBuffer());
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    
    submitInfo.pCommandBuffers = &cmdbuf;

    vkQueueSubmit(device->GetComputeQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->GetComputeQueue());
}

void Vulkan_BRDF_LUT::Destroy(){

}

VulkanTexture* Vulkan_BRDF_LUT::GetTextureLut(){
    return _brdf_lut_texture;
}