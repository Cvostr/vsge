#include "VulkanComputePipeline.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VulkanComputePipeline::VulkanComputePipeline() : 
	_pipeline(VK_NULL_HANDLE),
	_pipelineLayout(nullptr)
{}

bool VulkanComputePipeline::Create(VulkanShader* shader, VulkanPipelineLayout* layout){
    VkComputePipelineCreateInfo info = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
    info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    info.stage.module = shader->GetModules()[0].first;
    info.stage.pName = "main";
    info.layout = layout->GetPipelineLayout();

    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();
    vkCreateComputePipelines(device->getVkDevice(), nullptr, 1, &info, nullptr, &_pipeline);

    mCreated = true;

    return true;
}

void VulkanComputePipeline::Destroy(){
    if(mCreated){
        VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();
        vkDestroyPipeline(device->getVkDevice(), _pipeline, nullptr);
    }
}