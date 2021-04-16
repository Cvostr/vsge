#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vector>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {

    class VulkanInstance {
    private:
        VkDebugUtilsMessengerEXT debugMessenger;

        VkInstance mInstance;

        std::vector<VkExtensionProperties> inst_extensions;

        VkSurfaceKHR mSurface;

    public:
        bool init(bool validate, const char* app_name, int app_ver);

        VkInstance GetInstance() { return mInstance; }
        VkSurfaceKHR GetSurface() { return mSurface; }

        VulkanInstance();
    };

}