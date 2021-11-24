#include "VulkanMA.hpp"
#include "VulkanRAPI.hpp"
#include "VulkanCommandBuffer.hpp"
#include <Core/Threading/Thread.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

using namespace VSGE;

VulkanMA::VulkanMA(VulkanInstance* inst, VulkanDevice* device) {
    this->allocator = new VmaAllocator;

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorInfo.physicalDevice = device->getPhysicalDevice();
    allocatorInfo.device = device->getVkDevice();
    allocatorInfo.instance = inst->GetInstance();

    vmaCreateAllocator(&allocatorInfo, (VmaAllocator*)allocator);

    for (auto& family_prop : device->GetQueueFamilyProperties()) {
        VulkanCommandPool* cmdpool = new VulkanCommandPool;
        cmdpool->Create((uint32)_family_cmdpools.size());
        _family_cmdpools.push_back(cmdpool);
    }
}

void VulkanMA::allocate(const VkBufferCreateInfo createInfo, VkBuffer* buffer) {

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VmaAllocation allocation;
    vmaCreateBuffer(*((VmaAllocator*)allocator), &createInfo, &allocInfo, buffer, &allocation, nullptr);
}

void VulkanMA::allocate(VkBufferUsageFlags flags, VmaVkBuffer* buffer, uint32 size) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;

    VmaAllocationInfo stagingVertexBufferAllocInfo = {};
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, &buffer->Buffer, (VmaAllocation*)&buffer->_allocation, &stagingVertexBufferAllocInfo);
}

void VulkanMA::allocateCpu(VkBufferUsageFlags flags, VmaVkBuffer* buffer, uint32 size, void** mapped) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.usage = flags;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    //vbAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VmaAllocation tempBufferAlloc = VK_NULL_HANDLE;
    //VmaAllocationInfo tempBufferAllocInfo = {};
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, &buffer->Buffer, &tempBufferAlloc, nullptr);
    //*mapped = tempBufferAllocInfo.pMappedData;
    buffer->_allocation = tempBufferAlloc;

    map(buffer, mapped);
}

void VulkanMA::copy(VkBuffer buffer, uint32 offset, void* data, uint32 size) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    vbAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer tempBuffer = VK_NULL_HANDLE;
    VmaAllocation tempBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo tempBufferAllocInfo = {};
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, &tempBuffer, &tempBufferAlloc, &tempBufferAllocInfo);
    //Copy data to temporary buffer
    memcpy((char*)tempBufferAllocInfo.pMappedData + offset, data, size);

    ThreadCmdbufPair* pair = GetTransferCmdbufThreaded();
    pair->cmdbuf->Begin(true);
    pair->cmdbuf->CopyBuffer(tempBuffer, buffer, size);
    pair->cmdbuf->End();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    VkCommandBuffer cmdbuffer = pair->cmdbuf->GetCommandBuffer();
    submitInfo.pCommandBuffers = &cmdbuffer;

    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanDevice* device = vulkan_rapi->GetDevice();

    vkQueueSubmit(pair->transfer_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(pair->transfer_queue);

    vmaDestroyBuffer(*((VmaAllocator*)allocator), tempBuffer, tempBufferAlloc);
}

void VulkanMA::allocate(VkBufferUsageFlags flags, VmaVkBuffer* buffer, void* data, uint32 size) {
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = size;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};

    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | flags;
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;
    //Create GPU buffer
    vmaCreateBuffer(*((VmaAllocator*)allocator), &vbInfo, &vbAllocCreateInfo, &buffer->Buffer, (VmaAllocation*)&buffer->_allocation, nullptr);
    //Copy data to GPU buffer
    if(data != nullptr)
        copy(buffer->Buffer, 0, data, size);
}

void VulkanMA::map(VmaVkBuffer* buf, void** mem) {
    vmaMapMemory(*(VmaAllocator*)this->allocator, (VmaAllocation)buf->_allocation, mem);
}
void VulkanMA::unmap(VmaVkBuffer* buf) {
    vmaUnmapMemory(*(VmaAllocator*)this->allocator, (VmaAllocation)buf->_allocation);
}
void VulkanMA::destroyBuffer(VmaVkBuffer* buf) {
    vmaDestroyBuffer(*((VmaAllocator*)allocator), buf->Buffer, (VmaAllocation)buf->_allocation);
}

bool VulkanMA::createImage(VkImageCreateInfo* info, VmaVkImage* image) {
    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;
    

    if (vmaCreateImage(*((VmaAllocator*)allocator), info, &vbAllocCreateInfo, &image->Image, (VmaAllocation*)&image->_allocation, nullptr) != VK_SUCCESS)
        return false;


    return true;
}

void VulkanMA::destroyImage(VmaVkImage* image) {
    vmaDestroyImage(*((VmaAllocator*)allocator), image->Image, (VmaAllocation)image->_allocation);
}

ThreadCmdbufPair* VulkanMA::GetTransferCmdbufThreaded() {
    uint32 cur_thread = Thread::GetCurrentThreadID();

    for (auto& pair : _threaded_cmdbufs) {
        if (pair->thread_id == cur_thread)
            return pair;
    }

    VulkanDevice* device = VulkanRAPI::Get()->GetDevice();

    uint32 used_queues = (uint32)_threaded_cmdbufs.size();
    VkQueue queue = device->GetTransferQueue(used_queues);
    uint32 queue_family_index = device->GetTransferQueueFamilyIndex(used_queues);

    ThreadCmdbufPair* new_pair = new ThreadCmdbufPair;
    new_pair->thread_id = cur_thread;
    new_pair->transfer_queue = queue;
    new_pair->family_index = queue_family_index;
    new_pair->cmdbuf = new VulkanCommandBuffer;
    new_pair->cmdbuf->Create(_family_cmdpools[queue_family_index]);
    _threaded_cmdbufs.push_back(new_pair);
    return new_pair;
}