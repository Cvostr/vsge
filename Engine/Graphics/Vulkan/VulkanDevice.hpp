#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanInstance.hpp"
#include "../GpuObject.hpp"

namespace VSGE {

    class VulkanDevice : public IGpuObject {
    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mDevice; //logical device, created from physical device

        VkQueue mGraphicsQueue; //queue of logical device
        VkQueue mPresentQueue; //queue to present

        int32 GraphicsQueueFamilyIndex;
        int32 PresentQueueFamilyIndex;

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

        const std::string GetDeviceName();

        VkQueue GetGraphicsQueue();
        VkQueue GetPresentQueue();

        int32 GetGraphicsQueueFamilyIndex();
        int32 GetPresentQueueFamilyIndex();

        VulkanDevice();
    };

    VulkanDevice* CreatePrimaryDevice();
}