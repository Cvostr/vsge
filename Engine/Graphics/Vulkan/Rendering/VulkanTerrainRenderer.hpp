#pragma once

#include "../VulkanPipeline.hpp"

namespace VSGE{
    class VulkanTerrainRenderer{
    private:
        VulkanShader* _terrain_shader;
        VulkanPipelineLayout* _terrain_pipeline_layout;
        VulkanPipeline* _terrain_pipeline;

    public:
        void Create();
    };
}