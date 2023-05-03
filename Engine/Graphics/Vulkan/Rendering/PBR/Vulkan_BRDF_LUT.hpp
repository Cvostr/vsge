#pragma once

#include "../../VulkanComputePipeline.hpp"
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

namespace VSGE{
    class Vulkan_BRDF_LUT{
    private:
        VulkanComputePipeline* m_brdfPipeline;
        VulkanDescriptorPool* m_brdfDescriptorPool;
        VulkanDescriptorSet* m_brdfDescriptorSet;
        VulkanPipelineLayout* m_brdfPipelineLayout;
        VulkanShader* m_brdfLutShader;

        VulkanTexture* m_brdfLutMap;

        VulkanCommandPool* _brdf_cmdpool;
        VulkanCommandBuffer* _brdf_cmdbuffer;
    public:
        Vulkan_BRDF_LUT();

        ~Vulkan_BRDF_LUT();

        void Create();

        void Destroy();

        VulkanTexture* GetTextureLut();

        void RecordCmdbuffer(VulkanCommandBuffer* cmdbuf);
    };
}