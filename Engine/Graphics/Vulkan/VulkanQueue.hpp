#pragma once

#include <vulkan/vulkan.hpp>
#include <Core/VarTypes/Base.hpp>
#include <atomic>

namespace VSGE {

    typedef uint32 QueueFamilyTasksFlags;

    class VulkanQueueInfo {
    private:
        VkQueue queue;
        VkQueueFlags queueFlags;
        uint32 familyIndex;
        std::atomic<bool> acquired;
    public:

        bool supportsPresent;

        VkQueue GetQueue();
        VkQueueFlags GetFlags();
        uint32 GetFamilyIndex();
        void Acquire();
        bool IsAcquired();

        bool TryAcquire();
        void Release();

        VulkanQueueInfo();
        VulkanQueueInfo(VkQueue queue, uint32 familyIndex, VkQueueFlags queueFlags);
    };
}