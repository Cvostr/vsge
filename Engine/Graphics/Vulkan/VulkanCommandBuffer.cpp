#include "VulkanCommandBuffer.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkCommandPool VSGE::beginCommandPool() {
    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanDevice* device = vulkan_rapi->GetDevice();
    
    //Generate Command Pool
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = device->GetGraphicsQueueFamilyIndex();
    poolInfo.flags = 0; // Optional
    vkCreateCommandPool(device->getVkDevice(), &poolInfo, nullptr, &commandPool);

    return commandPool;
}

VkCommandBuffer VSGE::CreateSingleTimeComdbuf(VkCommandPool commandPool) {
    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanDevice* device = vulkan_rapi->GetDevice();

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device->getVkDevice(), &allocInfo, &commandBuffer);

    return commandBuffer;
}
void VSGE::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool) {
    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanDevice* device = vulkan_rapi->GetDevice();

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->GetGraphicsQueue());

    vkFreeCommandBuffers(device->getVkDevice(), commandPool, 1, &commandBuffer);
}