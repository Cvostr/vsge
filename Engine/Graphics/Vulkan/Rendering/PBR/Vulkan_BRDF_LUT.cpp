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
    _brdf_lut_texture->CreateImageView();

    _brdf_descr_pool = new VulkanDescriptorPool;
    _brdf_descr_pool->SetDescriptorSetsCount(1);
    _brdf_descr_pool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1);
    _brdf_descr_pool->Create();

    _brdf_descr_set = new VulkanDescriptorSet(_brdf_descr_pool);
    _brdf_descr_set->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
    _brdf_descr_set->Create();
    _brdf_descr_set->WriteDescriptorImage(0, _brdf_lut_texture, nullptr);

    _brdf_pipeline_layout = new VulkanPipelineLayout;
    _brdf_pipeline_layout->PushDescriptorSet(_brdf_descr_set);
    _brdf_pipeline_layout->Create();

    _brdf_pipeline = new VulkanComputePipeline;
    _brdf_pipeline->Create(_brdf_lut_shader, _brdf_pipeline_layout);

    _brdf_cmdpool = new VulkanCommandPool;
    _brdf_cmdpool->Create(device->GetGraphicsQueueFamilyIndex());

    _brdf_cmdbuffer = new VulkanCommandBuffer();
	_brdf_cmdbuffer->Create(_brdf_cmdpool);

    VkCommandBuffer cmdbuf = _brdf_cmdbuffer->GetCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(_brdf_cmdbuffer->GetCommandBuffer(), &beginInfo);
    //Copy buffer command
    _brdf_cmdbuffer->BindComputePipeline(*_brdf_pipeline);
    _brdf_cmdbuffer->BindDescriptorSets(*_brdf_pipeline_layout, 0, 1, _brdf_descr_set);
    _brdf_cmdbuffer->Dispatch(1024 / 32, 1024 / 32, 6);
    vkEndCommandBuffer(_brdf_cmdbuffer->GetCommandBuffer());
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    
    submitInfo.pCommandBuffers = &cmdbuf;

    vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
}

void Vulkan_BRDF_LUT::Destroy(){

}

VulkanTexture* Vulkan_BRDF_LUT::GetTextureLut(){
    return _brdf_lut_texture;
}