#pragma once

#include "../VulkanPipeline.hpp"
#include <Scene/EntityComponents/TerrainComponent.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>

#define UNI_ALIGN 256
#define TERRAIN_DATA_ELEM_SIZE 512
#define MAX_GRASS_TRANSFORMS 60000

namespace VSGE{

    class VulkanTerrainRenderer;

    struct VulkanTerrainGrassRange {
        uint32 offset;
        uint32 count;
    };

    class VulkanTerrain {
    private:
        VulkanDescriptorSet* _terrain_descr_set;
        std::vector<VulkanDescriptorSet*> _grass_descriptor_sets;
        TerrainComponent* _terrain;
    public:
        VulkanTerrain();
        ~VulkanTerrain();
        VulkanDescriptorSet* GetDescriptorSet();
        void SetTerrain(TerrainComponent* terrain, VulkanTerrainRenderer* terrain_renderer);
        void SetDescriptorTexture(Resource* texture, uint32 texture_type, uint32 texture_index);
        void SetDescriptorGrassTexture(Resource* texture, uint32 vegetable_index);
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

        VulkanShader* _grass_shader;
        VulkanPipelineLayout* _grass_pipeline_layout;
        VulkanPipeline* _grass_pipeline;

        VulkanSampler* _terrain_masks_sampler;
        VulkanSampler* _terrain_textures_sampler;
        VulkanBuffer* _terrains_buffer;
        VulkanBuffer* _grass_transform_buffer;
        VulkanMesh* _grass_mesh;

        VulkanRenderPass* _gbuffer_renderpass;
        VulkanTexture* _emptyZeroTexture;
        VulkanTexture* _emptyOneTexture;
        std::vector<VulkanDescriptorSet*>* _entity_descr_set;

        VulkanDescriptorPool* _terrains_descr_pool;
        VulkanDescriptorPool* _vegetables_descr_pool;

        std::vector<VulkanTerrain*> _terrains;
        uint32 _terrains_processed;
        uint32 _vegetables_transforms_written;

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
        VulkanDescriptorPool* GetTerrainDescriptorPool();
        VulkanDescriptorPool* GetGrassDescriptorPool();
        VulkanBuffer* GetGrassTransformsBuffer();
    };
}