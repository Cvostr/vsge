#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanDevice.hpp"
#include "../GpuObject.hpp"

namespace VSGE {

    typedef struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    }SwapChainSupportDetails;

    class VulkanSwapChain : public VSGE::IGpuObject {
    private:
        //Vulkan swapchain object
        VkSwapchainKHR mSwapChain;
        SwapChainSupportDetails SW_Details;
        //Images, created from swapchain images
        std::vector<VkImage> mSwapChainImages;
        std::vector<VkImageView> mSwapChainImageViews;
        //Surface format of created images
        VkSurfaceFormatKHR chosenSurfaceFormat;
        VkPresentModeKHR chosenPresentMode;

        VkExtent2D swap_extend;

        void CreateImages(VulkanDevice* Device, VkSurfaceFormatKHR ChosenSurfaceFormat);

    public:
        VkSurfaceFormatKHR GetChosenSurfaceFormat() { return chosenSurfaceFormat; }
        bool initSwapchain(VulkanDevice* Device, VulkanInstance* instance, int Width, int Height);

        void Destroy();

        VkSwapchainKHR GetSwapChain() { return mSwapChain; }
        uint32_t GetSwapChainImagesCount() { 
            return static_cast<uint32_t>(mSwapChainImages.size()); 
        }
        VkImageView GetImageViewAtIndex(unsigned int Index);
        VkExtent2D GetExtent() { return swap_extend; }

        VulkanSwapChain();
    };

}