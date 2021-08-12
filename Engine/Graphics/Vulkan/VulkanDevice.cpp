#include "VulkanDevice.hpp"
#include <Core/Logger.hpp>
#include "VulkanRAPI.hpp"

using namespace VSGE;

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    , "VK_EXT_extended_dynamic_state"
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

VkPhysicalDevice GetBestDevice(std::vector<VkPhysicalDevice>& devices){
    if(devices.size() == 0)
        return VK_NULL_HANDLE;
    //Try to find discrete gpu with highest performance
    VkPhysicalDevice best = VK_NULL_HANDLE;
    for(auto device : devices) {
        VkPhysicalDeviceProperties device_props;
        vkGetPhysicalDeviceProperties(device, &device_props);

        if(device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
            best = device;
        }
    }
    if(best != VK_NULL_HANDLE)
        return best;
    else{
       for(auto device : devices) {
        VkPhysicalDeviceProperties device_props;
        vkGetPhysicalDeviceProperties(device, &device_props);
        //check device name, it shouldnt be as "llvmpipe"
        if(device_props.deviceName[0] != 'l' && device_props.deviceName[1] != 'l'){
            return device;
        }
    }
    }
    return VK_NULL_HANDLE;
}

VulkanDevice* VSGE::CreatePrimaryDevice() {
    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanInstance* instance = vulkan_rapi->GetInstance();

    std::vector<VkPhysicalDevice> phys_devices_list;

    uint32_t gpus_count = 0;
    vkEnumeratePhysicalDevices(instance->GetInstance(), &gpus_count, nullptr);
    //resize vectors
    phys_devices_list.resize(gpus_count);
    vkEnumeratePhysicalDevices(instance->GetInstance(), &gpus_count, phys_devices_list.data());
    
    VkPhysicalDevice device = GetBestDevice(phys_devices_list);
    if(device != VK_NULL_HANDLE){
        VkPhysicalDeviceProperties device_props;
        vkGetPhysicalDeviceProperties(device, &device_props);

        VulkanDevice* Device = new VulkanDevice;
        Device->SetProperties(device_props);
        Logger::Log(LogType::LOG_TYPE_INFO) << "Creating Vulkan GPU " << device_props.deviceName << "\n";
        Device->initDevice(device);
        return Device;
    }
    
    return nullptr;
}

VulkanDevice::VulkanDevice() {

}

bool VulkanDevice::initDevice(VkPhysicalDevice Device) {
    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanInstance* instance = vulkan_rapi->GetInstance();

    mPhysicalDevice = Device;
    uint32_t vkQueueFamilyPropsCount = 0;
    std::vector<VkQueueFamilyProperties> vkQueueFamilyProps;
    //Obtain queue family props
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &vkQueueFamilyPropsCount, nullptr);
    vkQueueFamilyProps.resize(vkQueueFamilyPropsCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &vkQueueFamilyPropsCount, vkQueueFamilyProps.data());

    VkDeviceQueueCreateInfo QueueGraphicsCreateInfo = {};
    VkDeviceQueueCreateInfo QueuePresentCreateInfo = {};
    std::vector<VkDeviceQueueCreateInfo> QueuesToCreate;

    GraphicsQueueFamilyIndex = -1;
    PresentQueueFamilyIndex = -1;

    for (int32_t q_i = 0; q_i < (int32_t)vkQueueFamilyPropsCount; q_i++) {
        VkQueueFamilyProperties prop = vkQueueFamilyProps[q_i];
        if ((prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) && GraphicsQueueFamilyIndex < 0)
            GraphicsQueueFamilyIndex = q_i;
        VkBool32 canPresent = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(Device, q_i, instance->GetSurface(), &canPresent);
        if (canPresent && PresentQueueFamilyIndex < 0) {
            PresentQueueFamilyIndex = q_i;
        }
    }

    if (GraphicsQueueFamilyIndex >= 0 && PresentQueueFamilyIndex >= 0) { //if we found right queue family
        QueueGraphicsCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueGraphicsCreateInfo.pNext = nullptr;
        QueueGraphicsCreateInfo.queueFamilyIndex = static_cast<uint32_t>(GraphicsQueueFamilyIndex);
        QueueGraphicsCreateInfo.queueCount = 1;
        QueueGraphicsCreateInfo.flags = 0;
        float priority = 1.0f;
        QueueGraphicsCreateInfo.pQueuePriorities = &priority;

        QueuePresentCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueuePresentCreateInfo.pNext = nullptr;
        QueuePresentCreateInfo.queueFamilyIndex = static_cast<uint32_t>(PresentQueueFamilyIndex);
        QueuePresentCreateInfo.queueCount = 1;
        QueuePresentCreateInfo.flags = 0;
        QueuePresentCreateInfo.pQueuePriorities = &priority;
        //push create structs to vector
        QueuesToCreate.push_back(QueueGraphicsCreateInfo);
        QueuesToCreate.push_back(QueuePresentCreateInfo);
    }

    VkPhysicalDeviceFeatures features = {};
    features.geometryShader = true;
    features.multiViewport = true;
    features.shaderSampledImageArrayDynamicIndexing = true;
    features.imageCubeArray = true;
    features.independentBlend = true;
    features.samplerAnisotropy = true;
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT eds_features = {};
    eds_features.extendedDynamicState = true;

    VkDeviceCreateInfo logical_gpu_create_info = {};
    logical_gpu_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logical_gpu_create_info.pNext = nullptr;
    logical_gpu_create_info.flags = 0;
    logical_gpu_create_info.queueCreateInfoCount = static_cast<unsigned int>(QueuesToCreate.size()); //size of queues vector
    logical_gpu_create_info.pQueueCreateInfos = QueuesToCreate.data(); //pointer to start of queues vector
    logical_gpu_create_info.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size());
    logical_gpu_create_info.ppEnabledExtensionNames = &deviceExtensions[0];
    logical_gpu_create_info.pEnabledFeatures = &features;

    if (true) {
        logical_gpu_create_info.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
        logical_gpu_create_info.ppEnabledLayerNames = validationLayers.data();
    }

    //create logical device
    if (vkCreateDevice(Device, &logical_gpu_create_info, nullptr, &mDevice) != VK_SUCCESS) //creating logical device
        return false;                                                           
    //get graphics queue
    vkGetDeviceQueue(mDevice, static_cast<uint32_t>(GraphicsQueueFamilyIndex), 0, &this->mGraphicsQueue);
    //get present queue
    vkGetDeviceQueue(mDevice, static_cast<uint32_t>(PresentQueueFamilyIndex), 0, &this->mPresentQueue);

    mCreated = true;

    return true;
}

VkQueue VulkanDevice::GetGraphicsQueue() { return mGraphicsQueue; }
VkQueue VulkanDevice::GetPresentQueue() { return mPresentQueue; }

int32 VulkanDevice::GetGraphicsQueueFamilyIndex() { return GraphicsQueueFamilyIndex; }
int32 VulkanDevice::GetPresentQueueFamilyIndex() { return PresentQueueFamilyIndex; }

uint32 VulkanDevice::GetUniformBufferMinAlignment() {
    return (uint32)DeviceProps.limits.minUniformBufferOffsetAlignment;
}

uint32 VulkanDevice::GetMaxBoundDescriptorSets() {
    return DeviceProps.limits.maxBoundDescriptorSets;
}

VkFormat VulkanDevice::GetSuitableDepthFormat(VkImageTiling tiling){
    std::vector<VkFormat> depthFormats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};

    for(VkFormat format : depthFormats){
        if(IsCompatibleDepthFormatTiling(format, tiling)){
            return format;
        }
    }
    return VK_FORMAT_D32_SFLOAT;
}

bool VulkanDevice::IsCompatibleDepthFormatTiling(VkFormat format, VkImageTiling tiling){
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkFormatFeatureFlags tilingFormatFeatureFlags = 0;

    if(tiling == VK_IMAGE_TILING_LINEAR)
        tilingFormatFeatureFlags = props.linearTilingFeatures;
    else if(tiling == VK_IMAGE_TILING_OPTIMAL)
        tilingFormatFeatureFlags = props.optimalTilingFeatures;

    bool result = ((tilingFormatFeatureFlags & features) == features);

    return result;
}

const std::string VulkanDevice::GetDeviceName() {
    return DeviceProps.deviceName;
}

void VulkanDevice::Destroy() {
    if (mCreated) {
        vkDestroyDevice(mDevice, nullptr);
        mCreated = false;
    }
}