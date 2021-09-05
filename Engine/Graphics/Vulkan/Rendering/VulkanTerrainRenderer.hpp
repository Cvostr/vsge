#pragma once

#include "../VulkanPipeline.hpp"
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>

#define UNI_ALIGN 256
#define TERRAIN_DATA_ELEM_SIZE 512

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
        void SetDescriptorTexture(Resource* texture, uint32 texture_type, uint32 texture_index);
        TerrainComponent* GetTerrain();
        void Create(VulkanDescriptorPool* pool, VulkanBuffer* terrains_buffer = nullptr);
        void SetImagesToEmpty();
        void Destroy();
    };

    class VulkanTerrainRenderer{
    private:
        VulkanShader* _terrain_shader;
        VulkanPipelineLayout* _terrain_pipeline_layout;
        VulkanPipeline* _terrain_pipeline;
        VulkanSampler* _terrain_masks_sampler;
        VulkanSampler* _terrain_textures_sampler;
        VulkanBuffer* _terrains_buffer;

        VulkanRenderPass* _gbuffer_renderpass;
        VulkanTexture* _emptyZeroTexture;
        VulkanTexture* _emptyOneTexture;
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
            std::vector<VulkanDescriptorSet*>& descr_set,
            VulkanTexture* emptyZeroTexture,
            VulkanTexture* emptyOneTexture);
        void ProcessTerrain(Entity* terrain);
        void ResetProcessedTerrains();
        void DrawTerrain(VulkanCommandBuffer* cmdbuffer, uint32 terrain_index, uint32 draw_index);
        void SetOutputSizes(uint32 width, uint32 height);
        VulkanSampler* GetTerrainMasksTextureSampler();
        VulkanSampler* GetTerrainTextureSampler();
        VulkanTexture* GetEmptyZeroTexture();
        VulkanTexture* GetEmptyWhiteTexture();
    };
}