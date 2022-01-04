#pragma once

#include "../../../PostprocessEffect.hpp"
#include <Graphics/Vulkan/VulkanTexture.hpp>
#include <Graphics/Vulkan/VulkanBuffer.hpp>
#include <Graphics/Vulkan/VulkanShader.hpp>
#include <Graphics/Vulkan/VulkanDescriptors.hpp>
#include <Graphics/Vulkan/VulkanPipeline.hpp>

namespace VSGE {

	class VulkanSSAOBase {
	private:
		VulkanTexture* _ssao_noise;
		VulkanBuffer* _ssao_kernel;

		VulkanShader* _ssao_shader;
		VulkanShader* _ssao_blur_shader;

		VulkanPipelineLayout* _ssao_playout;
		VulkanPipelineLayout* _ssao_blur_playout;

		VulkanPipeline* _ssao_pipeline;
		VulkanPipeline* _ssao_blur_pipeline;

		void CreatePrecomputedSSAO();
	public:
		VulkanSSAOBase();
		~VulkanSSAOBase();

		void Create();
		void Destroy();

		VulkanBuffer* GetKernelBuffer();
		VulkanTexture* GetNoiseTexture();
		VulkanRenderPass* CreateSSAORenderPass();

		VulkanPipeline* GetSSAOPipeline();
		VulkanPipeline* GetSSAOBlurPipeline();
	};

	class VulkanSSAO : public PostprocessEffect {
	private:
		VulkanDescriptorPool* _descr_pool;

		VulkanDescriptorSet* _ssao_descr_set;
		VulkanDescriptorSet* _ssao_blur_descr_set;

		VulkanRenderPass* _rpass;
		VulkanFramebuffer* _fb_ssao;
		VulkanFramebuffer* _fb_ssao_blur;

		uint32 _camera_index;
	public:
		VulkanSSAO();
		~VulkanSSAO();

		void Create();
		void Destroy();

		void SetCameraIndex(uint32 camera_index);
		void SetInputTexture(Texture* input);
		void SetInputTextures(
			Texture* input_positions, 
			Texture* input_normals);
		void FillCommandBuffer(VulkanCommandBuffer* cmdbuf);
		void ResizeOutput(const Vec2i& new_size);
		VulkanTexture* GetBlurredSSAO();
	};
}