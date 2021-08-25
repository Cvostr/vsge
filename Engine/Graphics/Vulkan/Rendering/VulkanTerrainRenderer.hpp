#pragma once

#include "../VulkanPipeline.hpp"
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>

#define UNI_ALIGN 256

namespace VSGE{

    class VulkanTerrain {
    private:
        VulkanDescriptorSet* _terrain_descr_set;
        TerrainComponent* _terrain;
    public:
        VulkanTerrain();
        ~VulkanTerrain();
        VulkanDescriptorSet* GetDescriptorSet();
        void SetTerrain(TerrainComponent* terrain);
        TerrainComponent* GetTerrain();
        void Create(VulkanDescriptorPool* pool);
        void Destroy();
    };

    class VulkanTerrainRenderer{
    private:
        VulkanShader* _terrain_shader;
        VulkanPipelineLayout* _terrain_pipeline_layout;
        VulkanPipeline* _terrain_pipeline;

        VulkanRenderPass* _gbuffer_renderpass;
        std::vector<VulkanDescriptorSet*>* _entity_descr_set;

        VulkanDescriptorPool* _terrains_descr_pool;

        std::vector<VulkanTerrain*> _terrains;
        uint32 _terrains_processed;

        uint32 _outputWidth;
        uint32 _outputHeight;
    public:
        VulkanTerrainRenderer();
        ~VulkanTerrainRenderer();

        void Create(
            VulkanRenderPass* gbuffer_renderpass,
            std::vector<VulkanDescriptorSet*>& descr_set);
        void ProcessTerrain(Entity* terrain);
        void ResetProcessedTerrains();
        void DrawTerrain(VulkanCommandBuffer* cmdbuffer, uint32 terrain_index, uint32 draw_index);
        void SetOutputSizes(uint32 width, uint32 height);
    };
}