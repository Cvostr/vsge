#include "VulkanPipeline.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkDescriptorSet temp_sets[6];
VkBuffer temp_bufs[6];

VkCommandPool VSGE::beginCommandPool() {
    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanDevice* device = vulkan_rapi->GetDevice();
    
    //Generate Command Pool
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = device->GetGraphicsQueueFamilyIndex();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
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
    poolInfo.pNext = nullptr;
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

bool VulkanCommandBuffer::Create(VulkanCommandPool* pool) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool->GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)1;

    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanDevice* device = vulkan_rapi->GetDevice();

    if (vkAllocateCommandBuffers(device->getVkDevice(), &allocInfo, &mCommandBuffer) != VK_SUCCESS) {
        return false;
    }

    mPoolPtr = pool;
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

void VulkanCommandBuffer::BindPipeline(VulkanPipeline& pipeline) {
    vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());
}

void VulkanCommandBuffer::BindComputePipeline(VulkanComputePipeline& pipeline){
    vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipeline());
}

void VulkanCommandBuffer::BindDescriptorSets(VulkanPipelineLayout& layout, uint32 firstSet, uint32 setsCount, VulkanDescriptorSet* sets, uint32 dynOffsetCount, const uint32* offsets, VkPipelineBindPoint bind_point) {
    for (uint32 set_i = 0; set_i < setsCount; set_i++) {
        temp_sets[set_i] = sets[set_i].GetDescriptorSet();
    }
    vkCmdBindDescriptorSets(mCommandBuffer, bind_point, layout.GetPipelineLayout(), firstSet, setsCount, temp_sets, dynOffsetCount, offsets);
}

void VulkanCommandBuffer::PushConstants(VulkanPipelineLayout& layout, VkShaderStageFlagBits stage, uint32 offset, uint32 size, void* data) {
    vkCmdPushConstants(mCommandBuffer, layout.GetPipelineLayout(), stage, offset, size, data);
}

void VulkanCommandBuffer::BindVertexBuffer(VulkanBuffer& buffer) {
    VkDeviceSize offsets[] = { 0 };
    VkBuffer _buffer = buffer.GetBuffer();
    vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, &_buffer, offsets);
}

void VulkanCommandBuffer::BindVertexBuffers(VulkanBuffer** buffer, uint32 size, uint32 start) {
    VkDeviceSize offsets[] = { 0, 0, 0, 0  };
    for (uint32 buf_i = 0; buf_i < size; buf_i++) {
        temp_bufs[buf_i] = buffer[buf_i]->GetBuffer();
    }
    vkCmdBindVertexBuffers(mCommandBuffer, start, size, temp_bufs, offsets);
}

void VulkanCommandBuffer::BindIndexBuffer(VulkanBuffer& buffer) {
    vkCmdBindIndexBuffer(mCommandBuffer, buffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void VulkanCommandBuffer::BindMesh(VulkanMesh& mesh, uint32 firstSlot) {
    BindVertexBuffers(mesh.GetVertexBuffers(), mesh.GetVertexBuffersCount(), firstSlot);
    if(mesh.GetIndexBuffer())
        BindIndexBuffer(*mesh.GetIndexBuffer());
}

void VulkanCommandBuffer::Draw(uint32 vertices, uint32 instances, uint32 firstVertex, uint32 firstInstance) {
    vkCmdDraw(mCommandBuffer, vertices, instances, firstVertex, firstInstance);
}

void VulkanCommandBuffer::DrawIndexed(uint32 indices, uint32 instances, uint32 firstIndex, uint32 firstInstance) {
    vkCmdDrawIndexed(mCommandBuffer, indices, instances, firstIndex, 0, firstInstance);
}

void VulkanCommandBuffer::Dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ){
    vkCmdDispatch(mCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandBuffer::SetViewports(uint32 firstViewport, uint32 count, VkViewport* viewports) {
    vkCmdSetViewport(mCommandBuffer, firstViewport, count, viewports);
}

void VulkanCommandBuffer::SetViewport(float x, float y, float width, float height, float depthMin, float depthMax, uint32 index) {
    VkViewport viewport = {x, y, width, height, depthMin, depthMax };
    SetViewports(0, 1, &viewport);
    VkRect2D scissor = { 0, 0, (uint32)width, (uint32)height };
    vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
}

void VulkanCommandBuffer::SetCullMode(VkCullModeFlags cull_mode) {
    PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT = (PFN_vkCmdSetCullModeEXT)vkGetInstanceProcAddr(VulkanRAPI::Get()->GetInstance()->GetInstance(), "vkCmdSetCullModeEXT");
    vkCmdSetCullModeEXT(mCommandBuffer, cull_mode);
}

void VulkanCommandBuffer::ImagePipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, const std::vector<VkImageMemoryBarrier>& barriers){
    vkCmdPipelineBarrier(
        mCommandBuffer,
        srcStageMask,
        dstStageMask,
        0, 0, nullptr, 0, nullptr, (uint32)barriers.size(), barriers.data());
}

void VulkanCommandBuffer::EndRenderPass() {
    vkCmdEndRenderPass(mCommandBuffer);
}

void VulkanCommandBuffer::End() {
    vkEndCommandBuffer(mCommandBuffer);
}

void VulkanCommandBuffer::Destroy() {
    if (mCreated) {
        VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
        VulkanDevice* device = vulkan_rapi->GetDevice();

        vkFreeCommandBuffers(device->getVkDevice(), mPoolPtr->GetCommandPool(), 1, &mCommandBuffer);

        mCreated = false;
    }
}

VkImageMemoryBarrier VSGE::GetImageBarrier(VulkanTexture* texture, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = srcAccessMask;
		barrier.dstAccessMask = dstAccessMask;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = texture->GetImage();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
		return barrier;
	}