#pragma once

#include "../../../PostprocessEffect.hpp"
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanComputePipeline.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>

namespace VSGE {
	class VulkanBloom : public PostprocessEffect {
	private:
		VulkanShader* _bloom_bright_shader;

		VulkanDescriptorPool* _descr_pool;
		VulkanDescriptorSet* _bright_descr_set;
		
		VulkanComputePipeline* _bright_pipeline;
		VulkanPipelineLayout* _bright_pp_layout;

		VulkanTexture* _bright_texture;
		
	public:
		VulkanBloom();
		~VulkanBloom();

		void Create();
		void Destroy();

		void SetInputTexture(Texture* input);
		void FillCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
	};
}