#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanDevice.hpp"
#include "VulkanTexture.hpp"
#include "../GpuObject.hpp"

namespace VSGE {

    typedef struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;

        void Clear() {
            formats.clear();
            presentModes.clear();
        }
    }SwapChainSupportDetails;

    class VulkanSwapChain : public VSGE::IGpuObject {
    private:
        //Vulkan swapchain object
        VkSwapchainKHR mSwapChain;
        SwapChainSupportDetails SW_Details;
        //Images, created from swapchain images
        std::vector<VulkanTexture*> _swapchain_images;
        //Surface format of created images
        VkSurfaceFormatKHR chosenSurfaceFormat;
        VkPresentModeKHR chosenPresentMode;

        VkExtent2D swap_extend;

        void CreateImages(VulkanDevice* Device, VkSurfaceFormatKHR ChosenSurfaceFormat);

    public:
        VkSurfaceFormatKHR GetChosenSurfaceFormat() { return chosenSurfaceFormat; }
        bool initSwapchain(VulkanDevice* Device);

        void Destroy();

        VkSwapchainKHR GetSwapChain();
        uint32 GetSwapChainImagesCount();
        VulkanTexture* GetImageAtIndex(uint32 index);
        VkExtent2D GetExtent() { return swap_extend; }

        VulkanSwapChain();

        ~VulkanSwapChain();
    };

}