#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vector>
#include <Core/VarTypes/Base.hpp>
#include "../GpuObject.hpp"

namespace VSGE {

    class VulkanInstance : public IGpuObject {
    private:
        VkDebugUtilsMessengerEXT debugMessenger;

        VkInstance mInstance;

        std::vector<VkExtensionProperties> inst_extensions;

        VkSurfaceKHR mSurface;

    public:
        bool init(bool validate, const char* app_name, int app_ver);
        void Destroy();

        VkInstance GetInstance() { return mInstance; }
        VkSurfaceKHR GetSurface() { return mSurface; }

        VulkanInstance();
    };

}