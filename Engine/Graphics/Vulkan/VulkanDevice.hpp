#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanInstance.hpp"
#include "../GpuObject.hpp"

namespace VSGE {

    class VulkanDevice : public IGpuObject {
    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mDevice; //logical device, created from physical device

        VkQueue _graphicsQueue; //queue of logical device
        VkQueue _presentQueue; //queue to present
        VkQueue _computeQueue;

        int32 _graphicsQueueFamilyIndex;
        int32 _presentQueueFamilyIndex;
        int32 _computeQueueFamilyIndex;

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

        VkQueue GetGraphicsQueue();
        VkQueue GetPresentQueue();
        VkQueue GetComputeQueue();

        int32 GetGraphicsQueueFamilyIndex();
        int32 GetPresentQueueFamilyIndex();
        int32 GetComputeQueueFamilyIndex();

        VulkanDevice();
    };

    VulkanDevice* CreatePrimaryDevice();
}