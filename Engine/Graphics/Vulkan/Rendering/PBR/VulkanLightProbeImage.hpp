#pragma once

#include <Graphics/LightProbeImage.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>

namespace VSGE {

	class VulkanLightProbeImage : public LightProbeImage {
	private:
		VulkanTexture* _spmap;
		VulkanTexture* _irmap;

		uint32		max_resolution;
		uint32_t	mipmaps;

		// For direct rendering
		VulkanDescriptorPool* _descr_pool;
		VulkanDescriptorSet* _irmap_descr_set;
		VulkanDescriptorSet* _spmap_descr_set;
		std::vector<VkImageView> _spmapImageViews;

	public:

		VulkanLightProbeImage();
		~VulkanLightProbeImage();

		void Create();
		void CreateForRendering();

		void Destroy();

		VulkanDescriptorSet* GetIrmapDescrSet();
		VulkanDescriptorSet* GetSpmapDescrSet();

		Texture* GetSpmap();
		Texture* GetIrmap();

		uint32 GetSpMaxResolution();
		uint32 GetSpMipmapLevelsNum();
	};
}