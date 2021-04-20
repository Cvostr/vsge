#include "VulkanShader.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkShaderStageFlagBits VSGE::GetShaderStageVK(ShaderStagesBits stage) {
    switch (stage) {
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

void VulkanShader::AddShader(byte* shaderData, uint32 shaderSize, ShaderStagesBits type) {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();
    
    VkShaderModule shader_module;
    
    VkShaderModuleCreateInfo createVsInfo = {};
    createVsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createVsInfo.pNext = nullptr;
    createVsInfo.codeSize = shaderSize;
    createVsInfo.pCode = reinterpret_cast<const uint32_t*>(shaderData);
    if (vkCreateShaderModule(device->getVkDevice(), &createVsInfo, nullptr, &shader_module) != VK_SUCCESS) {
        return;
    }

    mModules.push_back(VulkanShaderPair(shader_module, type));

    mStages = (ShaderStagesBits)(mStages | type);
}

void VulkanShader::Destroy() {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();

    for (auto shaderPair : mModules) {
        VkShaderModule shader_module = shaderPair.first;
        vkDestroyShaderModule(device->getVkDevice(), shader_module, nullptr);
    }
}