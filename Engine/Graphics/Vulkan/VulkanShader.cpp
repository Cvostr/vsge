#include "VulkanShader.hpp"
#include "VulkanRAPI.hpp"

using namespace VSGE;

VkShaderStageFlagBits VSGE::GetShaderStageVK(ShaderTypesBits stage) {
    switch (stage) {
    case VERT_SHADER:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case FRAG_SHADER:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case GEOM_SHADER:
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    case TESSCTRL_SHADER:
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case TESSEVAL_SHADER:
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }
    return VK_SHADER_STAGE_ALL;
}

void VulkanShader::AddShader(byte* shaderData, uint32 shaderSize, ShaderTypesBits type) {
    VulkanRAPI* vulkan = VulkanRAPI::Get();
    VulkanDevice* device = vulkan->GetDevice();
    
    VkShaderModule shader_module;
    
    VkShaderModuleCreateInfo createVsInfo = {};
    createVsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createVsInfo.pNext = nullptr;
    createVsInfo.codeSize = shaderSize;
    createVsInfo.pCode = reinterpret_cast<const uint32_t*>(shaderData);
    vkCreateShaderModule(device->getVkDevice(), &createVsInfo, nullptr, &shader_module);

    mModules.push_back(VulkanShaderPair(shader_module, type));

    mTypes |= type;
}