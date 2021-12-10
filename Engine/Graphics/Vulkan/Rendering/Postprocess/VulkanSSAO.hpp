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
		VulkanShader* _ssao_blur_shader;
		
		VulkanDescriptorPool* _descr_pool;
		VulkanDescriptorSet* _ssao_descr_set;
		VulkanDescriptorSet* _ssao_blur_descr_set;

		VulkanComputePipeline* _ssao_pipeline;
		VulkanPipelineLayout* _ssao_pp_layout;

		VulkanComputePipeline* _ssao_blur_pipeline;
		VulkanPipelineLayout* _ssao_blur_pp_layout;

		VulkanTexture* _ssao_texture;
		VulkanTexture* _ssao_blur_texture;

		void CreatePrecomputedSSAO();

		void UpdateProjectionMatrix(uint32 cam_index = 0);
	public:
		VulkanSSAO();
		~VulkanSSAO();

		void Create();
		void Destroy();

		void SetInputTexture(Texture* input);
		void SetInputTextures(
			Texture* input_positions, 
			Texture* input_normals);
		void FillCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
		VulkanTexture* GetBlurredSSAO();
	};
}