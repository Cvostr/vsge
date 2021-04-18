#include "VulkanPipelineLayout.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

bool VulkanPipelineLayout::Create() {
    VkPipelineLayoutCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    pipeline_info.setLayoutCount = 0;
    pipeline_info.pSetLayouts = nullptr;
    pipeline_info.pushConstantRangeCount =
        static_cast<uint32_t>(mPushConstants.size());
    pipeline_info.pPushConstantRanges = mPushConstants.data();

    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    if (vkCreatePipelineLayout(device->getVkDevice(), &pipeline_info, nullptr, &this->mPLayout) != VK_SUCCESS)
        return false;

    mCreated = true;

    return true;
}

void VulkanPipelineLayout::Destroy() {
    if (mCreated) {
        VulkanRAPI* vulkan = VulkanRAPI::Get();
        VulkanDevice* device = vulkan->GetDevice();
        vkDestroyPipelineLayout(device->getVkDevice(), mPLayout, nullptr);
        mCreated = false;
    }
}