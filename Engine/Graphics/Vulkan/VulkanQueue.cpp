#include <Graphics/Vulkan/VulkanQueue.hpp>

using namespace VSGE;

VulkanQueueInfo::VulkanQueueInfo() : 
    queue(VK_NULL_HANDLE),
    familyIndex(0),
    queueFlags(0),
    supportsPresent(false),
    acquired(false) {}

VulkanQueueInfo::VulkanQueueInfo(VkQueue queue, uint32 familyIndex, VkQueueFlags queueFlags) :
    queue(queue),
    familyIndex(familyIndex),
    queueFlags(queueFlags),
    supportsPresent(false),
    acquired(false) {}

VkQueue VulkanQueueInfo::GetQueue() {
    return queue;
}

VkQueueFlags VulkanQueueInfo::GetFlags() {
    return queueFlags;
}

uint32 VulkanQueueInfo::GetFamilyIndex() {
    return familyIndex;
}

void VulkanQueueInfo::Acquire() {
    acquired = true;
}

bool VulkanQueueInfo::IsAcquired() {
    return acquired;
}

bool VulkanQueueInfo::TryAcquire()
{
    bool expected = false;
    bool desired = true;
    return acquired.compare_exchange_strong(expected, desired);
}

void VulkanQueueInfo::Release()
{
    acquired = false;
}