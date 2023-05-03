#include "Vulkan_BRDF_LUT.hpp"
#include "../../VulkanRAPI.hpp"

using namespace VSGE;

Vulkan_BRDF_LUT::Vulkan_BRDF_LUT() :
    m_brdfLutMap(nullptr),
    m_brdfLutShader(nullptr),
    m_brdfPipeline(nullptr),
    m_brdfPipelineLayout(nullptr),
    m_brdfDescriptorSet(nullptr),
    m_brdfDescriptorPool(nullptr)
{}

Vulkan_BRDF_LUT::~Vulkan_BRDF_LUT(){

}

void Vulkan_BRDF_LUT::Create(){
    VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

    m_brdfLutShader = new VulkanShader;
    m_brdfLutShader->AddShaderFromFile("brdf_lut.comp", SHADER_STAGE_COMPUTE);

    m_brdfLutMap = new VulkanTexture;
    m_brdfLutMap->SetStorage(true);
    m_brdfLutMap->Create(1024, 1024, FORMAT_RG16F);

    m_brdfDescriptorPool = new VulkanDescriptorPool;
    m_brdfDescriptorPool->SetDescriptorSetsCount(1);
    m_brdfDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1);
    m_brdfDescriptorPool->Create();

    m_brdfDescriptorSet = new VulkanDescriptorSet(m_brdfDescriptorPool);
    m_brdfDescriptorSet->AddDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, VK_SHADER_STAGE_COMPUTE_BIT);
    m_brdfDescriptorSet->Create();
    m_brdfDescriptorSet->WriteDescriptorImage(0, m_brdfLutMap, nullptr, VK_IMAGE_LAYOUT_GENERAL);

    m_brdfPipelineLayout = new VulkanPipelineLayout;
    m_brdfPipelineLayout->PushDescriptorSet(m_brdfDescriptorSet);
    m_brdfPipelineLayout->Create();

    m_brdfPipeline = new VulkanComputePipeline;
    m_brdfPipeline->Create(m_brdfLutShader, m_brdfPipelineLayout);

    _brdf_cmdpool = new VulkanCommandPool;
    _brdf_cmdpool->Create(device->GetGenericQueueFamilyIndex());

    _brdf_cmdbuffer = new VulkanCommandBuffer();
	_brdf_cmdbuffer->Create(_brdf_cmdpool);

    VkCommandBuffer cmdbuf = _brdf_cmdbuffer->GetCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(_brdf_cmdbuffer->GetCommandBuffer(), &beginInfo);
    RecordCmdbuffer(_brdf_cmdbuffer);
    vkEndCommandBuffer(_brdf_cmdbuffer->GetCommandBuffer());
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    
    submitInfo.pCommandBuffers = &cmdbuf;

    vkQueueSubmit(device->GetGenericQueue()->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->GetGenericQueue()->GetQueue());
}

void Vulkan_BRDF_LUT::RecordCmdbuffer(VulkanCommandBuffer* cmdbuf) {
    VkImageMemoryBarrier pre_brdf_barrier = GetImageBarrier(m_brdfLutMap, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    VkImageMemoryBarrier post_brdf_barrier = GetImageBarrier(m_brdfLutMap, VK_ACCESS_SHADER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _brdf_cmdbuffer->ImagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, { pre_brdf_barrier });
    _brdf_cmdbuffer->BindComputePipeline(*m_brdfPipeline);
    _brdf_cmdbuffer->BindDescriptorSets(*m_brdfPipelineLayout, 0, 1, m_brdfDescriptorSet, 0, nullptr, VK_PIPELINE_BIND_POINT_COMPUTE);
    _brdf_cmdbuffer->Dispatch(1024 / 32, 1024 / 32, 6);
    _brdf_cmdbuffer->ImagePipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { post_brdf_barrier });
}

void Vulkan_BRDF_LUT::Destroy(){

}

VulkanTexture* Vulkan_BRDF_LUT::GetTextureLut(){
    return m_brdfLutMap;
}