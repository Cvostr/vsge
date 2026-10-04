#pragma once

#include "../VulkanPipeline.hpp"
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include "../VulkanTerrain.hpp"

namespace VSGE {

    class VulkanTerrainRenderer {
    private:
       

        VulkanDescriptorPool* _terrains_descr_pool;
        VulkanDescriptorPool* _vegetables_descr_pool;
        uint32 _terrains_processed;
        uint32 _vegetables_transforms_written;
    public:
        VulkanTerrainRenderer();
        ~VulkanTerrainRenderer();
        void Create(
            VulkanRenderPass* gbuffer_renderpass,
            VulkanDescriptorSet* transform_descr_set);
        void ProcessTerrain(Entity* terrain);
        void ResetProcessedTerrains();
        static void DrawTerrain(
            VulkanCommandBuffer* cmdbuffer,
            TerrainComponent* component,
            uint32 camera_index,
            VulkanDescriptorSet* entity_descr_set,
            uint32 draw_index,
            uint32 output_width,
            uint32 output_height,
            bool reverse_cull);
        void SetOutputSizes(uint32 width, uint32 height);

        VulkanDescriptorPool* GetTerrainDescriptorPool();
        VulkanDescriptorPool* GetGrassDescriptorPool();
        uint32& GetWrittenGrassTransforms();
    };
}