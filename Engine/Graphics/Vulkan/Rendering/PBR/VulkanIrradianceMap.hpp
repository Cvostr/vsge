#pragma once

#include "../../VulkanComputePipeline.hpp"
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

namespace VSGE {
	class VulkanIrradianceMap {
	private:
		VulkanShader* _irmap_shader;
		VulkanComputePipeline* _irmap_pipeline;
		VulkanPipelineLayout* _irmap_pipeline_layout;

		VulkanDescriptorPool* _irmap_descr_pool;
		VulkanDescriptorSet* _irmap_descr_set;

		VulkanTexture* _irmap_output_texture;
	public:
		void Create();

		VulkanTexture* GetIrradianceMap();
	};
}