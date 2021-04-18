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

bool VulkanCommandPool::Create(uint32 queueFamilyIndex) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanDevice* device = vulkan_rapi->GetDevice();

    if (vkCreateCommandPool(device->getVkDevice(), &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS) {
        return false;
    }

    mCreated = true;

    return true;
}

void VulkanCommandPool::Destroy() {
    if (mCreated) {
        VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
        VulkanDevice* device = vulkan_rapi->GetDevice();

        vkDestroyCommandPool(device->getVkDevice(), mCommandPool, nullptr);

        mCreated = false;
    }
}

bool VulkanCommandBuffer::Create(VulkanCommandPool& pool) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool.GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)1;

    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanDevice* device = vulkan_rapi->GetDevice();

    if (vkAllocateCommandBuffers(device->getVkDevice(), &allocInfo, &mCommandBuffer) != VK_SUCCESS) {
        return false;
    }

    mCreated = true;

    return true;
}

void VulkanCommandBuffer::Begin() {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    vkBeginCommandBuffer(mCommandBuffer, &beginInfo);
}

void VulkanCommandBuffer::End() {
    vkEndCommandBuffer(mCommandBuffer);
}

void VulkanCommandBuffer::Destroy() {
    if (mCreated) {
        VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
        VulkanDevice* device = vulkan_rapi->GetDevice();

        

        mCreated = false;
    }
}