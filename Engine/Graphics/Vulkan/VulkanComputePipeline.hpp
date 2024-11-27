#pragma once

#include <Graphics/GpuObject.hpp>
#include <vulkan/vulkan.hpp>
#include "VulkanPipelineLayout.hpp"
#include "VulkanShader.hpp"

namespace VSGE 
{
    class VulkanComputePipeline : public IGpuObject {
    private:
        VkPipeline _pipeline;
        VulkanPipelineLayout* _pipelineLayout;
    public:

        VulkanComputePipeline();
        VulkanComputePipeline(const VulkanComputePipeline&) = delete;
        ~VulkanComputePipeline();

        VkPipeline GetPipeline();

        VulkanPipelineLayout* GetPipelineLayout();

        bool Create(VulkanShader* shader, VulkanPipelineLayout* layout);
		void Destroy();
    };
}