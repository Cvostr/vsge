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

		VulkanTexture* _spmap_output_texture;
	public:

		void Create();
	};
}