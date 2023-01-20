#include <Graphics/Vulkan/VulkanQueue.hpp>

using namespace VSGE;

VulkanQueueInfo::VulkanQueueInfo() : 
    queue(VK_NULL_HANDLE),
    familyIndex(0),
    queueFlags(0),
    supportsPresent(false),
    acquiredByThread(false) {}

VulkanQueueInfo::VulkanQueueInfo(VkQueue queue, uint32 familyIndex, VkQueueFlags queueFlags) :
    queue(queue),
    familyIndex(familyIndex),
    queueFlags(queueFlags),
    supportsPresent(false),
    acquiredByThread(false) {}

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
    acquiredByThread = true;
}

bool VulkanQueueInfo::IsAcquired() {
    return acquiredByThread;
}