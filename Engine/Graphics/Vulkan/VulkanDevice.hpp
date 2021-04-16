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

    public:
        bool initDevice(VkPhysicalDevice Device, VulkanInstance* instance);

        VkDevice getVkDevice() { return mDevice; }
        VkPhysicalDevice getPhysicalDevice() { return mPhysicalDevice; }

        VkQueue GetGraphicsQueue() { return mGraphicsQueue; }
        VkQueue GetPresentQueue() { return mPresentQueue;  }

        int32_t GetGraphicsQueueFamilyIndex() { return GraphicsQueueFamilyIndex; }
        int32_t GetPresentQueueFamilyIndex() { return PresentQueueFamilyIndex; }

        VulkanDevice();
    };

    VulkanDevice* CreatePrimaryDevice(VulkanInstance* instance);
}