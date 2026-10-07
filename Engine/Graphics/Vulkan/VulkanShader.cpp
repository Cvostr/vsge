#include "VulkanShader.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VulkanShader::~VulkanShader() {
    Destroy();
}

VkShaderStageFlagBits VSGE::GetShaderStageVK(ShaderStagesBits stage) 
{
    switch (stage) 
    {
    case SHADER_STAGE_VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case SHADER_STAGE_FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case SHADER_STAGE_GEOMETRY:
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    case SHADER_STAGE_COMPUTE:
        return VK_SHADER_STAGE_COMPUTE_BIT;
    case SHADER_STAGE_TESSELATION_CTRL:
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case SHADER_STAGE_TESSELATION_EVAL:
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }
    return VK_SHADER_STAGE_ALL;
}

uint32 VulkanShader::GetModulesCount() const
{ 
    return static_cast<uint32>(mModules.size());
}

VulkanShaderPair* VulkanShader::GetModules() 
{ 
    return mModules.data(); 
}

bool VulkanShader::AddShader(byte* shaderData, size_t shaderSize, ShaderStagesBits type) 
{
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();
    
    VkShaderModule shader_module;
    
    VkShaderModuleCreateInfo createShaderInfo = {};
    createShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createShaderInfo.pNext = nullptr;
    createShaderInfo.codeSize = shaderSize;
    createShaderInfo.pCode = reinterpret_cast<const uint32_t*>(shaderData);

    if (vkCreateShaderModule(device->getVkDevice(), &createShaderInfo, nullptr, &shader_module) != VK_SUCCESS) {
        return false;
    }

    mModules.push_back(VulkanShaderPair(shader_module, type));

    mStages = (ShaderStagesBits)(mStages | type);
    return true;
}

void VulkanShader::Destroy() {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    for (auto shaderPair : mModules) {
        VkShaderModule shader_module = shaderPair.first;
        vkDestroyShaderModule(device->getVkDevice(), shader_module, nullptr);
    }
}