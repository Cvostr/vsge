#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanInstance.hpp"
#include "../GpuObject.hpp"

namespace VSGE {

    enum QueueFamilyTask {
        TASK_GRAPHICS,
        TASK_COMPUTE,
        TASK_TRANSFER,
        TASK_PRESENT
    };

    struct QueueFamilyIndexPair {
        VkQueue queue;
        uint32 familyIndex;

        QueueFamilyIndexPair() : queue(VK_NULL_HANDLE), familyIndex(0) {}
        QueueFamilyIndexPair(VkQueue queue, uint32 familyIndex) :
            queue(queue), familyIndex(familyIndex) {}
    };

    class VulkanDevice : public IGpuObject {
    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mDevice; //logical device, created from physical device
        std::vector<VkQueueFamilyProperties> _vkQueueFamilyProps;
        std::vector<VkQueue> _queues;

        QueueFamilyIndexPair _present_queue;
        std::vector<QueueFamilyIndexPair> _graphics_queues;
        std::vector<QueueFamilyIndexPair> _compute_queues;
        std::vector<QueueFamilyIndexPair> _transfer_queues;

        bool CheckQueueFamilySupport(uint32 family_index, QueueFamilyTask task);
        VkQueue GetQueueByIndexInFamily(uint32 family_index, uint32 queue_index);

        VkPhysicalDeviceProperties DeviceProps;

    public:
        bool initDevice(VkPhysicalDevice Device);
        void Destroy();

        VkDevice getVkDevice() { return mDevice; }
        VkPhysicalDevice getPhysicalDevice() { return mPhysicalDevice; }

        const VkPhysicalDeviceProperties& GetProps() { return DeviceProps; }
        void SetProperties(const VkPhysicalDeviceProperties& DeviceProps) { this->DeviceProps = DeviceProps; }

        uint32 GetUniformBufferMinAlignment();
        uint32 GetMaxBoundDescriptorSets();
        VkFormat GetSuitableDepthFormat(VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);
        bool IsCompatibleDepthFormatTiling(VkFormat format, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);

        const std::string GetDeviceName();

        VkQueue GetGraphicsQueue(uint32 index = 0);
        VkQueue GetPresentQueue();
        VkQueue GetComputeQueue(uint32 index = 0);
        VkQueue GetTransferQueue(uint32 index = 0);

        uint32 GetGraphicsQueueFamilyIndex(uint32 index = 0);
        uint32 GetPresentQueueFamilyIndex();
        uint32 GetComputeQueueFamilyIndex(uint32 index = 0);
        uint32 GetTransferQueueFamilyIndex(uint32 index = 0);

        std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties();

        VulkanDevice();
    };

    VulkanDevice* CreatePrimaryDevice();
}