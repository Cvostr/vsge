#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanInstance.hpp"

namespace VSGE {

    class VulkanDevice {
    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mDevice; //logical device, created from physical device

        VkQueue mGraphicsQueue; //queue of logical device
        VkQueue mPresentQueue; //queue to present

        int32_t GraphicsQueueFamilyIndex;
        int32_t PresentQueueFamilyIndex;

        VkPhysicalDeviceProperties DeviceProps;

    public:
        bool initDevice(VkPhysicalDevice Device);

        VkDevice getVkDevice() { return mDevice; }
        VkPhysicalDevice getPhysicalDevice() { return mPhysicalDevice; }

        const VkPhysicalDeviceProperties& GetProps() { return DeviceProps; }
        void SetProperties(const VkPhysicalDeviceProperties& DeviceProps) { this->DeviceProps = DeviceProps; }

        VkQueue GetGraphicsQueue() { return mGraphicsQueue; }
        VkQueue GetPresentQueue() { return mPresentQueue;  }

        int32_t GetGraphicsQueueFamilyIndex() { return GraphicsQueueFamilyIndex; }
        int32_t GetPresentQueueFamilyIndex() { return PresentQueueFamilyIndex; }

        VulkanDevice();
    };

    VulkanDevice* CreatePrimaryDevice();
}