#pragma once

#include "../../VulkanComputePipeline.hpp"
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>

namespace VSGE {
	class VulkanSpecularMap {
	private:
		VulkanShader* _spmap_shader;
		VulkanComputePipeline* _spmap_pipeline;
		VulkanPipelineLayout* _spmap_pipeline_layout;

		VulkanDescriptorPool* _spmap_descr_pool;
		VulkanDescriptorSet* _spmap_descr_set;

		VulkanTexture* _spmap_output_texture;

		VulkanCommandPool* _spmap_cmdpool;
		VulkanCommandBuffer* _spmap_cmdbuffer;
	public:

		void Create();

		void FillCommandBuffer();
	};
}