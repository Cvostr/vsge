#include "VulkanPipelineLayout.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkPipelineLayout VulkanPipelineLayout::GetPipelineLayout() { 
    return _pipelineLayout; 
}

void VulkanPipelineLayout::PushDescriptorSet(VulkanDescriptorSet* set) { 
    _descriptor_sets.push_back(set); 
}

void VulkanPipelineLayout::AddPushConstantRange(uint32 offset, uint32 size, VkShaderStageFlags stage){
    VkPushConstantRange range = {};
    range.offset = offset;
    range.size = size;
    range.stageFlags = stage;
    _pushConstants.push_back(range);
}

bool VulkanPipelineLayout::Create() {
    std::vector<VkDescriptorSetLayout> layouts;

    for (auto set : _descriptor_sets) {
        layouts.push_back(set->GetDescriptorSetLayout());
    }

    VkPipelineLayoutCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    pipeline_info.setLayoutCount = static_cast<uint32>(layouts.size());
    pipeline_info.pSetLayouts = layouts.data();
    pipeline_info.pushConstantRangeCount =
        static_cast<uint32_t>(_pushConstants.size());
    pipeline_info.pPushConstantRanges = _pushConstants.data();

    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    if (vkCreatePipelineLayout(device->getVkDevice(), &pipeline_info, nullptr, &this->_pipelineLayout) != VK_SUCCESS)
        return false;

    mCreated = true;

    return true;
}

void VulkanPipelineLayout::Destroy() {
    if (mCreated) {
        VulkanRAPI* vulkan = VulkanRAPI::Get();
        VulkanDevice* device = vulkan->GetDevice();
        vkDestroyPipelineLayout(device->getVkDevice(), _pipelineLayout, nullptr);
        mCreated = false;
    }
}