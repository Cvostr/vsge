#pragma once

#include <Graphics/Terrain.hpp>
#include "VulkanPipeline.hpp"
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Scene/EntityComponents/TerrainComponent.hpp>

namespace VSGE {

    class VulkanTerrainGrass {
    public:
        VulkanDescriptorSet* _descr_set;
        uint32 offset;
        uint32 count;

        VulkanTerrainGrass(VulkanDescriptorSet* descrSet);
        VulkanTerrainGrass(VulkanTerrainGrass &&other) noexcept;
        ~VulkanTerrainGrass();
    };

    class VulkanTerrain : public Terrain {
    private:
        VulkanBuffer* info_buffer;
        VulkanBuffer* grass_transform_buffer;
        VulkanDescriptorSet* _terrain_descr_set;
        std::vector<VulkanTerrainGrass> _grass_descriptor_sets;
        TerrainComponent* m_boundComponent;
    public:
        
        VulkanTerrain();
        ~VulkanTerrain();

        VulkanDescriptorSet* GetDescriptorSet();
        void UpdateInfo(TerrainComponent* terrain);
        std::vector<VulkanTerrainGrass>& GetVegetables();
        void SetTerrain(TerrainComponent* terrain, VulkanDescriptorPool *pool);
        void SetDescriptorTexture(Resource* texture, uint32 texture_type, uint32 texture_index);
        void SetDescriptorGrassTexture(Resource* texture, uint32 vegetable_index);
        void Create(VulkanDescriptorPool* pool);
        void SetImagesToEmpty();
        void Destroy();
    };
}