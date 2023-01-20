#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanInstance.hpp"
#include "../GpuObject.hpp"
#include "VulkanQueue.hpp"

namespace VSGE {

    class VulkanDevice : public IGpuObject {
    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mDevice; //logical device, created from physical device
        std::vector<VkQueueFamilyProperties> _vkQueueFamilyProps;

        VulkanQueueInfo _generic_queue; //This Queue can either present, draw graphics, compute and transfer

        std::vector<VulkanQueueInfo> _queues;

        VkPhysicalDeviceProperties DeviceProps;

    public:
        bool initDevice(VkPhysicalDevice Device);
        void Destroy();

        VkDevice getVkDevice();
        VkPhysicalDevice getPhysicalDevice();

        const VkPhysicalDeviceProperties& GetProps() { return DeviceProps; }
        void SetProperties(const VkPhysicalDeviceProperties& DeviceProps) { this->DeviceProps = DeviceProps; }

        uint32 GetUniformBufferMinAlignment();
        uint32 GetMaxBoundDescriptorSets();
        VkFormat GetSuitableDepthFormat(VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);
        bool IsCompatibleDepthFormatTiling(VkFormat format, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);

        const std::string GetDeviceName();

        //VkQueue GetGenericQueue();
        uint32 GetGenericQueueFamilyIndex();
        VulkanQueueInfo* GetGenericQueue();

        VulkanQueueInfo* GetFreeQueue(VkQueueFlags flags);

        std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties();

        VulkanDevice();
    };

    VulkanDevice* CreatePrimaryDevice();
}