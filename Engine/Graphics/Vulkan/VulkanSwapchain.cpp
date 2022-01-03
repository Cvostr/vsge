#include "VulkanSwapchain.hpp"
#include <Core/Logger.hpp>
#include "VulkanRAPI.hpp"
#include <Engine/Window.hpp>

using namespace VSGE;

const VkImageUsageFlags usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

VulkanSwapChain::VulkanSwapChain() :
    mSwapChain(VK_NULL_HANDLE)
{
    chosenSurfaceFormat = {};
    chosenPresentMode = {};
    swap_extend = {};
}

VulkanSwapChain::~VulkanSwapChain() {
    Destroy();
}

VulkanTexture* VulkanSwapChain::GetImageAtIndex(uint32 index) {
    if (index > _swapchain_images.size())
        index = static_cast<uint32_t>(_swapchain_images.size());
    return _swapchain_images[index];
}

bool VulkanSwapChain::initSwapchain(VulkanDevice* Device) {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanInstance* instance = vulkan->GetInstance();
    
    
    SwapChainSupportDetails result;
    //Get surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device->getPhysicalDevice(), instance->GetSurface(), &result.capabilities);

    uint32_t surface_format_count = 0;
    //Get surface formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device->getPhysicalDevice(), instance->GetSurface(), &surface_format_count, nullptr);

    result.formats.resize(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device->getPhysicalDevice(), instance->GetSurface(), &surface_format_count, result.formats.data());

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device->getPhysicalDevice(), instance->GetSurface(), &present_mode_count, nullptr);

    result.presentModes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device->getPhysicalDevice(), instance->GetSurface(), &present_mode_count, result.presentModes.data());


    for (unsigned int i = 0; i < result.formats.size(); i++) {
        VkSurfaceFormatKHR format = result.formats[i];
        if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_UNORM) {
            chosenSurfaceFormat = format;
        }
    }
    //find best presentation mode
    for (unsigned int i = 0; i < result.presentModes.size(); i++) {
        VkPresentModeKHR pres_mode = result.presentModes[i];
        if (pres_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            chosenPresentMode = pres_mode;
    }

    Window* win = Window::Get();

    swap_extend.width = static_cast<uint32>(win->GetWindowWidth());
    swap_extend.height = static_cast<uint32>(win->GetWindowHeight());

    //Now fill the strcucture
    VkSwapchainCreateInfoKHR swc_create_info;
    swc_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swc_create_info.pNext = nullptr;
    swc_create_info.flags = 0;
    swc_create_info.surface = instance->GetSurface();
    swc_create_info.minImageCount = result.capabilities.minImageCount + 1;
    swc_create_info.imageExtent = swap_extend;
    //Configure image props
    swc_create_info.imageFormat = chosenSurfaceFormat.format;
    swc_create_info.imageColorSpace = chosenSurfaceFormat.colorSpace;

    swc_create_info.imageArrayLayers = 1;
    swc_create_info.imageUsage = usage_flags;

    swc_create_info.queueFamilyIndexCount = 2;
    uint32 queueFamilyIndices[] = { 
        (uint32)Device->GetGraphicsQueueFamilyIndex(), 
        (uint32)Device->GetPresentQueueFamilyIndex() 
    };
    swc_create_info.pQueueFamilyIndices = queueFamilyIndices;

    if (Device->GetGraphicsQueueFamilyIndex() != Device->GetPresentQueueFamilyIndex()) { //if indices are not equal
        swc_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        //TODO - implement
    }
    else {
        swc_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swc_create_info.queueFamilyIndexCount = 0; // Optional
        swc_create_info.pQueueFamilyIndices = nullptr; // Optional
    }
    //No extra transformations
    swc_create_info.preTransform = result.capabilities.currentTransform;
    //No composite alpha
    swc_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swc_create_info.presentMode = chosenPresentMode;
    swc_create_info.clipped = VK_TRUE;
    swc_create_info.oldSwapchain = VK_NULL_HANDLE;

    Logger::Log(LogType::LOG_TYPE_INFO) << "Creating Vulkan Swapchain" << "\n";
    //Creating swapchain
    if (vkCreateSwapchainKHR(Device->getVkDevice(), &swc_create_info, nullptr, &this->mSwapChain) != VK_SUCCESS) {
        return false;
    }
    Logger::Log(LogType::LOG_TYPE_INFO) << "Vulkan: Swapchain creation successful" << "\n";

    CreateImages(Device, chosenSurfaceFormat);

    mCreated = true;

    return true;
}

void VulkanSwapChain::Destroy() {
    VkDevice device = VulkanRAPI::Get()->GetDevice()->getVkDevice();
    if (mCreated) {
        //Destroy created image views
        for (uint32_t img_i = 0; img_i < GetSwapChainImagesCount(); img_i++) {
            vkDestroyImageView(device, _swapchain_images[img_i]->GetImageView(), nullptr);
            delete _swapchain_images[img_i];
        }
        //Clear image view array
        _swapchain_images.clear();
        SW_Details.Clear();
        //Destroy Swapchain
        vkDestroySwapchainKHR(device, mSwapChain, nullptr);

        mCreated = false;
    }
}

VkSwapchainKHR VulkanSwapChain::GetSwapChain() {
    return mSwapChain; 
}

uint32 VulkanSwapChain::GetSwapChainImagesCount() {
    return static_cast<uint32>(_swapchain_images.size());
}

void VulkanSwapChain::CreateImages(VulkanDevice* Device, VkSurfaceFormatKHR ChosenSurfaceFormat) {
    uint32_t swc_images;
    std::vector<VkImage> swapchain_images;
    vkGetSwapchainImagesKHR(Device->getVkDevice(), this->mSwapChain, &swc_images, nullptr);
    swapchain_images.resize(swc_images);
    vkGetSwapchainImagesKHR(Device->getVkDevice(), this->mSwapChain, &swc_images, swapchain_images.data());

    //Iterate over all swapchain images and create image views
    for (uint32 sw_i = 0; sw_i < swc_images; sw_i++) {
        VkImageViewCreateInfo img_view_create_info;
        img_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        img_view_create_info.pNext = nullptr;
        img_view_create_info.flags = 0;
        img_view_create_info.image = swapchain_images[sw_i];
        img_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        img_view_create_info.format = ChosenSurfaceFormat.format;

        img_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        img_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        img_view_create_info.subresourceRange.baseMipLevel = 0;
        img_view_create_info.subresourceRange.levelCount = 1;
        img_view_create_info.subresourceRange.baseArrayLayer = 0;
        img_view_create_info.subresourceRange.layerCount = 1;

        VkImageView imageView = VK_NULL_HANDLE;
        vkCreateImageView(Device->getVkDevice(), &img_view_create_info, nullptr, &imageView);

        VulkanTexture* texture = new VulkanTexture(
            swapchain_images[sw_i],
            nullptr,
            imageView,
            swap_extend.width,
            swap_extend.height,
            1,
            1,
            usage_flags,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        texture->SetSwapchain(true);
        _swapchain_images.push_back(texture);
    }
}