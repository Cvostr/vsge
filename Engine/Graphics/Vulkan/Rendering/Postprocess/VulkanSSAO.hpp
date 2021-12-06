#pragma once

#include <Graphics/PostprocessEffect.hpp>
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanComputePipeline.hpp>

namespace VSGE {
	class VulkanSSAO : public PostprocessEffect {
	private:
		VulkanTexture* _ssao_noise;
		VulkanBuffer* _ssao_kernel;

		VulkanShader* _ssao_shader;
		
		VulkanDescriptorPool* _descr_pool;
		VulkanDescriptorSet* _ssao_descr_set;

		VulkanComputePipeline* _ssao_pipeline;
		VulkanPipelineLayout* _ssao_pp_layout;

		void CreatePrecomputedSSAO();
	public:
		VulkanSSAO();
		~VulkanSSAO();

		void Create();
		void Destroy();

		void SetInputTexture(Texture* input);
		void SetInputTextures(Texture* input_positions, Texture* input_normals);
		void FillCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
	};
}