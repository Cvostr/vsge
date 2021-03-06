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

#define GRAPHICS_QUEUES_NEEDED 1
#define COMPUTE_QUEUES_NEEDED 2
#define TRANSFER_QUEUES_NEEDED 4

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
    
    //Obtain queue family props
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &vkQueueFamilyPropsCount, nullptr);
    _vkQueueFamilyProps.resize(vkQueueFamilyPropsCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &vkQueueFamilyPropsCount, _vkQueueFamilyProps.data());

    std::vector<VkDeviceQueueCreateInfo> QueuesToCreate;
    float priority = 1.0f;
    for (uint32 i = 0; i < _vkQueueFamilyProps.size(); i++) {
        VkDeviceQueueCreateInfo QueueGraphicsCreateInfo = {};
        QueueGraphicsCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueGraphicsCreateInfo.pNext = nullptr;
        QueueGraphicsCreateInfo.queueFamilyIndex = i;
        QueueGraphicsCreateInfo.queueCount = _vkQueueFamilyProps[i].queueCount;
        QueueGraphicsCreateInfo.flags = 0;
       
        float* priorities_array = new float[_vkQueueFamilyProps[i].queueCount];
        for (uint32 _i = 0; _i < _vkQueueFamilyProps[i].queueCount; _i++)
            priorities_array[_i] = 1.f;

        QueueGraphicsCreateInfo.pQueuePriorities = priorities_array;

        QueuesToCreate.push_back(QueueGraphicsCreateInfo);
    }

    VkPhysicalDeviceFeatures features = {};
    features.geometryShader = true;
    features.multiViewport = true;
    features.shaderSampledImageArrayDynamicIndexing = true;
    features.imageCubeArray = true;
    features.independentBlend = true;
    features.samplerAnisotropy = true;
    features.depthClamp = true;
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT eds_features = {};
    eds_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    eds_features.extendedDynamicState = true;

    VkDeviceCreateInfo logical_gpu_create_info = {};
    logical_gpu_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logical_gpu_create_info.pNext = &eds_features;
    logical_gpu_create_info.flags = 0;
    logical_gpu_create_info.queueCreateInfoCount = static_cast<uint32>(QueuesToCreate.size()); //size of queues vector
    logical_gpu_create_info.pQueueCreateInfos = QueuesToCreate.data(); //pointer to start of queues vector
    logical_gpu_create_info.enabledExtensionCount = static_cast<uint32>(deviceExtensions.size());
    logical_gpu_create_info.ppEnabledExtensionNames = &deviceExtensions[0];
    logical_gpu_create_info.pEnabledFeatures = &features;

    if (true) {
        logical_gpu_create_info.enabledLayerCount = static_cast<uint32>(validationLayers.size());
        logical_gpu_create_info.ppEnabledLayerNames = validationLayers.data();
    }

    //create logical device
    if (vkCreateDevice(Device, &logical_gpu_create_info, nullptr, &mDevice) != VK_SUCCESS) //creating logical device
        return false;                                                           
  
    for (uint32 family_i = 0; family_i < _vkQueueFamilyProps.size(); family_i++) {
        
        for (uint32 q_i = 0; q_i < _vkQueueFamilyProps[family_i].queueCount; q_i++) {
            VkQueue queue;
            vkGetDeviceQueue(mDevice, family_i, q_i, &queue);

            if (CheckQueueFamilySupport(family_i, TASK_PRESENT) && _present_queue.queue == VK_NULL_HANDLE) {
                _present_queue = QueueFamilyIndexPair(queue, family_i);
                continue;
            }
            if (_graphics_queues.size() < GRAPHICS_QUEUES_NEEDED && CheckQueueFamilySupport(family_i, TASK_GRAPHICS)) {
                QueueFamilyIndexPair pair(queue, family_i);
                _graphics_queues.push_back(pair);
                continue;
            }
            if (_compute_queues.size() < COMPUTE_QUEUES_NEEDED && CheckQueueFamilySupport(family_i, TASK_COMPUTE)) {
                QueueFamilyIndexPair pair(queue, family_i);
                _compute_queues.push_back(pair);
                continue;
            }
            if (_transfer_queues.size() < TRANSFER_QUEUES_NEEDED && CheckQueueFamilySupport(family_i, TASK_TRANSFER)) {
                QueueFamilyIndexPair pair(queue, family_i);
                _transfer_queues.push_back(pair);
                continue;
            }

            _queues.push_back(queue);
        }
    }

    mCreated = true;

    return true;
}

bool VulkanDevice::CheckQueueFamilySupport(uint32 family_index, QueueFamilyTask task) {
    VulkanRAPI* vulkan_rapi = VulkanRAPI::Get();
    VulkanInstance* instance = vulkan_rapi->GetInstance();
    VkQueueFamilyProperties prop = _vkQueueFamilyProps[family_index];
    
    if (task == TASK_GRAPHICS && prop.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        return true;

    if (task == TASK_COMPUTE && prop.queueFlags & VK_QUEUE_COMPUTE_BIT)
        return true;

    if (task == TASK_TRANSFER && prop.queueFlags & VK_QUEUE_TRANSFER_BIT)
        return true;

    if (task == TASK_PRESENT) {
        VkBool32 canPresent = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, family_index, instance->GetSurface(), &canPresent);
        if (canPresent) {
            return true;
        }
    }

    return false;
}

VkQueue VulkanDevice::GetQueueByIndexInFamily(uint32 family_index, uint32 queue_index) {
    uint32 first_queue_index = 0;
    for (uint32 i = 0; i < family_index; i++) {
        first_queue_index += _vkQueueFamilyProps[i].queueCount;
    }
    return _queues[first_queue_index + queue_index];
}

VkQueue VulkanDevice::GetGraphicsQueue(uint32 index) { 
    return _graphics_queues[index].queue; 
}
VkQueue VulkanDevice::GetPresentQueue() { 
    return _present_queue.queue; 
}
VkQueue VulkanDevice::GetComputeQueue(uint32 index) { 
    return _compute_queues[index].queue;
}
VkQueue VulkanDevice::GetTransferQueue(uint32 index) {
    return _transfer_queues[index].queue;
}

uint32 VulkanDevice::GetGraphicsQueueFamilyIndex(uint32 index) { 
    return _graphics_queues[index].familyIndex;
}
uint32 VulkanDevice::GetPresentQueueFamilyIndex() { 
    return _present_queue.familyIndex; 
}
uint32 VulkanDevice::GetComputeQueueFamilyIndex(uint32 index) { 
    return _compute_queues[index].familyIndex; 
}
uint32 VulkanDevice::GetTransferQueueFamilyIndex(uint32 index) {
    return _transfer_queues[index].familyIndex;
}

std::vector<VkQueueFamilyProperties>& VulkanDevice::GetQueueFamilyProperties() {
    return _vkQueueFamilyProps;
}

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