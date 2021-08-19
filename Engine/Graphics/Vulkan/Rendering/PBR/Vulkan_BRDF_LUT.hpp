#pragma once

#include "../../VulkanComputePipeline.hpp"
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

namespace VSGE{
    class Vulkan_BRDF_LUT{
    private:
        VulkanComputePipeline* _brdf_pipeline;
        VulkanDescriptorPool* _brdf_descr_pool;
        VulkanDescriptorSet* _brdf_descr_set;
        VulkanPipelineLayout* _brdf_pipeline_layout;
        VulkanShader* _brdf_lut_shader;

        VulkanTexture* _brdf_lut_texture;

        VulkanCommandPool* _brdf_cmdpool;
        VulkanCommandBuffer* _brdf_cmdbuffer;
    public:
        Vulkan_BRDF_LUT();

        ~Vulkan_BRDF_LUT();

        void Create();

        void Destroy();

        void CreateBrdfLut();

        VulkanTexture* GetTextureLut();
    };
}